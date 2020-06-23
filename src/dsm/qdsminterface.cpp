/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of QtTWAIN project https://git.semlanik.org/semlanik/QtTWAIN
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "qdsminterface.h"

#include <QString>
#include <QDebug>
#include <QLibrary>

namespace {
static const char *DSMEntryFunctionName = "DSM_Entry";
}

namespace QtTWAIN {

class QDSMInterfacePrivate final {
public:
    bool load(const QString &fullLibraryPath) {
        if (DSMEntryFunction != nullptr) {
            qDebug() << "TWAIN DSM is already loaded";
            return true;
        }

        mLibInstance.setFileName(fullLibraryPath);
        if (!mLibInstance.load()) {
            qCritical() << "Unable to load TWAIN DSM : " << fullLibraryPath << " : " << mLibInstance.errorString();
            return false;
        }
        auto *symbol = mLibInstance.resolve(DSMEntryFunctionName);

        if (symbol == nullptr) {
            qCritical() << "Unable to resolve TWAIN DSM entry point : " << fullLibraryPath << " : " << mLibInstance.errorString();
            return false;
        }

        DSMEntryFunction = reinterpret_cast<DSMENTRYPROC>(symbol);
    }
    void unload() {
        DSMEntryFunction = nullptr;
        mLibInstance.unload();
    }

    DSMENTRYPROC DSMEntryFunction = nullptr;
    TW_ENTRYPOINT DSMEntryStructure = {0, 0, 0, 0, 0, 0};
private:
    QLibrary mLibInstance;
};

}

using namespace QtTWAIN;

QDSMInterface::QDSMInterface() : dPtr(new QDSMInterfacePrivate)
{
    dPtr->load("TWAINDSM.dll");
}

QDSMInterface::~QDSMInterface()
{
    delete dPtr;
}

TW_UINT16 QDSMInterface::entry(pTW_IDENTITY origin, pTW_IDENTITY destination, TW_UINT32 dg, TW_UINT16 dat, TW_UINT16 msg, TW_MEMREF data)
{
    if (dPtr->DSMEntryFunction == nullptr) {
        return TWRC_FAILURE;
    }
    return dPtr->DSMEntryFunction(origin, destination, dg, dat, msg, data);
}

TW_HANDLE QDSMInterface::alloc(TW_UINT32 size)
{
    if (dPtr->DSMEntryStructure.DSM_MemAllocate != nullptr) {
        return dPtr->DSMEntryStructure.DSM_MemAllocate(size);
    }
#ifdef TWH_CMP_MSC
    return ::GlobalAlloc(GPTR, size);
#else
    return 0;
#endif
}

void QDSMInterface::free(TW_HANDLE handle)
{
    if (dPtr->DSMEntryStructure.DSM_MemFree)
    {
        return dPtr->DSMEntryStructure.DSM_MemFree(handle);
    }

#ifdef TWH_CMP_MSC
    ::GlobalFree(handle);
#endif
}

TW_MEMREF QDSMInterface::lock(TW_HANDLE handle)
{
    if (dPtr->DSMEntryStructure.DSM_MemLock) {
        return dPtr->DSMEntryStructure.DSM_MemLock(handle);
    }

#ifdef TWH_CMP_MSC
    return (TW_MEMREF)::GlobalLock(handle);
#else
    return 0;
#endif
}

void QDSMInterface::unlock(TW_HANDLE handle)
{
    if (dPtr->DSMEntryStructure.DSM_MemUnlock) {
        dPtr->DSMEntryStructure.DSM_MemUnlock(handle);
        return;
    }
#ifdef TWH_CMP_MSC
    ::GlobalUnlock(handle);
#endif
}

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

#pragma once

#include <memory>

#include <qtwain_p.h>

namespace QtTWAIN {

class QDSMInterfacePrivate;

class QDSMInterface
{
public:
    QDSMInterface();
    virtual ~QDSMInterface();

    //TODO: temporary singleton
    static QDSMInterface *instance() {
        static QDSMInterface instance;
        return &instance;
    }

    TW_UINT16 entry(pTW_IDENTITY origin,
                     pTW_IDENTITY destination,
                     TW_UINT32 dg,
                     TW_UINT16 dat,
                     TW_UINT16 msg,
                     TW_MEMREF data);

    TW_HANDLE alloc(TW_UINT32 size);
    void free(TW_HANDLE handle);

    TW_MEMREF lock(TW_HANDLE handle);
    void unlock(TW_HANDLE handle);

private:
    QDSMInterfacePrivate *dPtr;
};

}

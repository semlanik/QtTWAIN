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

#include <QHash>
#include <qwindowdefs.h>
#include <memory>

#include "qtwain_p.h"

#include "qtwaineventfilter_p.h"
#include "dsm/qdsminterface.h"

#include <type_traits>

namespace QtTWAIN {

class QTWAINScanner;
class QTWAINEventFilter;

class QTWAINContext {
public:
    static QTWAINContext *instance() {
        static QTWAINContext ctx;
        return &ctx;
    }

    void openDSM(const WId winId);
    void closeDSM();
    void fetchScannerList();

    QList<QString> scannerList() const {
        return m_scannerList.keys();
    }

    QtTWAIN::QTWAINScanner *scanner(const QString& name) const {
        return m_scannerList.value(name, std::shared_ptr<QtTWAIN::QTWAINScanner>(nullptr)).get();
    }

    bool openDS(TW_IDENTITY *dataSource);
    void closeDS(TW_IDENTITY *dataSource);
    bool enableDS(TW_IDENTITY *dataSource);
    bool disableDS(TW_IDENTITY *dataSource);
    bool imageInfo(TW_IMAGEINFO *info, TW_IDENTITY *dataSource);

    bool processEvent(TW_EVENT *event, TW_IDENTITY *dataSource);
    quint16 imageNativeTransfer(TW_HANDLE *imgHandle, TW_IDENTITY *dataSource);
    bool endTransfer(TW_PENDINGXFERS *pendingTransfers, TW_IDENTITY *dataSource);

    void updateStatus(TW_IDENTITY *dataSource = nullptr);
private:
    QTWAINContext();
    ~QTWAINContext();
    Q_DISABLE_COPY_MOVE(QTWAINContext)

    enum States {
        Initial,
        DSMOpen,
    };

    QDSMInterface *m_dsm;
    States m_state;

    TW_MEMREF m_windowHandle;
    TW_IDENTITY m_appIdentity; /**< current application identity */
    TW_STATUS m_status;

    QHash<QString, std::shared_ptr<QtTWAIN::QTWAINScanner>> m_scannerList;
};

}

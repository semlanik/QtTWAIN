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

#include <QObject>

#include "qtwain_p.h"

#include "qtwainglobal.h"
#include <queue>

namespace QtTWAIN {

class QTWAINContext;
class QTWAINEventFilter;
class QTWAINScanner;
class QTWAINScannerPrivate : public QObject
{
    Q_OBJECT

public:
    QTWAINScannerPrivate(QTWAINContext *ctx);

    bool scan();

    bool open();
    void close();

    QTWAINContext *context;
    TW_IDENTITY dataSource;
    QtTWAIN::Status status;
    QTWAINScanner *qPtr;

    bool processEvent(void *message);

signals:
    void processEvents();

private:
    bool transferImage();
    bool m_isOpen;
    std::unique_ptr<QTWAINEventFilter> m_filter;
    Q_DISABLE_COPY_MOVE(QTWAINScannerPrivate)
    std::queue<TW_EVENT> m_events;
};

}

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
#include <QImage>

#include "qtwainglobal.h"

#include <memory>

namespace QtTWAIN {
class QTWAINScannerPrivate;
class QTWAINContext;

class QTWAINScanner final : public QObject
{
    Q_OBJECT
public:
    quint32 id() const;

    quint16 versionMajor() const;
    quint16 versionMinor() const;

    quint16 protocolMajor() const;
    quint16 protocolMinor() const;

    QString manufacturer() const;
    QString family() const;
    QString name() const;

    bool isDefault() const;

    QtTWAIN::Status status() const;

    bool scan();

    virtual ~QTWAINScanner() = default;//TODO: move to protected section

signals:
    void imageReady(const QImage &);

protected:
    friend class QTWAINContext;
    friend class QTWAINScannerManager;
    QTWAINScanner(QTWAINScannerPrivate *d);

private:
    QTWAINScanner() = default;
    Q_DISABLE_COPY_MOVE(QTWAINScanner)
    std::unique_ptr<QTWAINScannerPrivate> dPtr;
};
}

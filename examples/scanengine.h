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

namespace QtTWAIN {
class QTWAINScanner;
}

class ScanEngine : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString scannerName READ scannerName CONSTANT)
    Q_PROPERTY(QString scannedImage READ scannedImage NOTIFY scannedImageChanged)

public:
    ScanEngine();
    virtual ~ScanEngine();

    Q_INVOKABLE void scan();
    QString scannerName() const;

    QString scannedImage() const {
        return m_scannedImage;
    }

signals:
    void scannedImageChanged();

private:
    QtTWAIN::QTWAINScanner *m_scanner;
    QString m_scannedImage;
};
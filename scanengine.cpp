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

#include "scanengine.h"

#include "qtwainscannermanager.h"
#include "qtwainscanner.h"

#include <QBuffer>
#include <QDebug>

ScanEngine::ScanEngine() : m_scanner(nullptr)
{
    QList<QString> scanners = QtTWAIN::QTWAINScannerManager::scannerList();
    qDebug() << scanners;
    if (scanners.size() > 0) {
        m_scanner = QtTWAIN::QTWAINScannerManager::acquireScanner(scanners.at(0));
        QObject::connect(m_scanner, &QtTWAIN::QTWAINScanner::imageReady, this, [this](const QImage &img) {
            QByteArray imgData;
            QBuffer buff(&imgData);
            buff.open(QIODevice::WriteOnly);
            img.save(&buff, "PNG");
            m_scannedImage = imgData.toBase64();
            scannedImageChanged();
        });
    }
}

ScanEngine::~ScanEngine() {
    if (m_scanner != nullptr) {
        QtTWAIN::QTWAINScannerManager::releaseScanner(m_scanner);
        disconnect(m_scanner);
    }
}

QString ScanEngine::scannerName() const
{
    if (m_scanner == nullptr) {
        return {};
    }

    return m_scanner->name();
}

void ScanEngine::scan() {
    m_scanner->scan();
}

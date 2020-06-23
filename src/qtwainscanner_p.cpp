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

#include "qtwainscanner_p.h"
#include "qtwainscanner.h"

#include "qtwaincontext_p.h"

#include <QDebug>
#include <QIODevice>
#include <QDataStream>
#include <QImage>

using namespace QtTWAIN;

QTWAINScannerPrivate::QTWAINScannerPrivate(QTWAINContext *ctx) : context(ctx)
  , status(QtTWAIN::Unknown)//TODO: init with unknown once complete statuses handling
  , qPtr(nullptr)
  , m_isOpen(false)
  , m_filter(nullptr)
{
    memset(&dataSource, 0, sizeof(dataSource));
    connect(this, &QTWAINScannerPrivate::processEvents, this, [this] {
        qDebug() << "ProcessEvents";
        while (!m_events.empty()) {
            auto event = m_events.front();
            qDebug() << "Scanner event received: " << QString("0x%1").arg(QString::number(event.TWMessage, 16));
            switch(event.TWMessage)
            {
            case MSG_CLOSEDSREQ:
                context->disableDS(&dataSource);
                break;
            case MSG_XFERREADY:
                transferImage();
                break;
            case MSG_CLOSEDSOK:
                qDebug() << "MSG_CLOSEDSOK";
                break;
            case MSG_DEVICEEVENT:
                qDebug() << "MSG_DEVICEEVENT";
                break;
            default:
                qDebug() << "Unhandled";
                break;
            }
            m_events.pop();
        }
        qDebug() << "Exit ProcessEvents";
    }, Qt::QueuedConnection);
}

bool QTWAINScannerPrivate::scan()
{
    if (status != QtTWAIN::Ready) {
        qCritical() << "Scanning is already started";
        return false;
    }

    return context->enableDS(&dataSource);
}

bool QTWAINScannerPrivate::open()
{
    if (m_isOpen) {
        return true;
    }

    if (context->openDS(&dataSource)) {
        m_filter.reset(new QTWAINEventFilter(this));
        m_isOpen = true; //TODO: probably might be handled by state
        status = QtTWAIN::Ready; //TODO: call status changed notification
    } else {
        status = QtTWAIN::Error;
    }
}

void QTWAINScannerPrivate::close()
{
    if (!m_isOpen) {
        qDebug() << "Trying to close scanner that is not opened, skipping";
        return;
    }
    m_filter.reset(nullptr);
    context->closeDS(&dataSource);
    status = QtTWAIN::Unknown;
}

bool QTWAINScannerPrivate::processEvent(void *message)
{
    TW_EVENT event{message, 0};
    if (!context->processEvent(&event, &dataSource)) {
        return false;
    }

    m_events.push(event);
    processEvents();
    qDebug() << "Exit processEvent";
    return true;
}

bool QTWAINScannerPrivate::transferImage()
{
    //Getting image information
    TW_IMAGEINFO info;
    memset(&info, 0, sizeof(info));
    if (!context->imageInfo(&info, &dataSource)) {
        qCritical() << "Unable to receive image tranfer information";
        return false;
    }

    qDebug() << "image size" << info.ImageWidth << "x" << info.ImageLength;
    TW_HANDLE imageHandle = nullptr;
    //Perform a Native transfer
    quint16 result = context->imageNativeTransfer(&imageHandle, &dataSource);//Not sure probably it's blocking call or cause other issues
    qDebug() << "Transfer result:" << QString::number(result, 16);
    if (TWRC_SUCCESS == result || TWRC_XFERDONE == result) {
        PBITMAPINFOHEADER dib = static_cast<PBITMAPINFOHEADER>(QtTWAIN::QDSMInterface::instance()->lock(imageHandle));
        if (dib != nullptr) {
            QByteArray bmpData;
            QDataStream ds(&bmpData, QIODevice::WriteOnly);

            int offset  = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*dib->biClrUsed);
            int dibSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*dib->biClrUsed) + dib->biSizeImage;
            if(BI_RGB == dib->biCompression) {
                //Note: Calculate the size of the image manually when uncompressed - do not trust the biSizeImage member
                dibSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*dib->biClrUsed) + ((((dib->biWidth * dib->biBitCount + 31)/32) * 4) * dib->biHeight);
            }

            BITMAPFILEHEADER header;
            memset(&header, 0, sizeof(BITMAPFILEHEADER));
            header.bfType    = ( (WORD) ('M' << 8) | 'B');
            header.bfOffBits = offset;
            header.bfSize    = sizeof(BITMAPFILEHEADER) + dibSize;
            ds.writeRawData(reinterpret_cast<char *>(&header), sizeof(BITMAPFILEHEADER));
            ds.writeRawData(reinterpret_cast<char *>(dib), dibSize);

            QtTWAIN::QDSMInterface::instance()->unlock(imageHandle);
            QtTWAIN::QDSMInterface::instance()->free(imageHandle);

#if 1
            QImage img;
            img.loadFromData(bmpData, "BMP");
            if (qPtr != nullptr) {
                qPtr->imageReady(img);
            }
#endif
        }

        TW_PENDINGXFERS pendingTransfers;
        memset(&pendingTransfers, 0, sizeof(pendingTransfers));

        return context->endTransfer(&pendingTransfers, &dataSource);
    }
    return false;
}

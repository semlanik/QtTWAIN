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

#include "qtwainscanner.h"

#include "qtwaincontext_p.h"
#include "qtwainscanner_p.h"

#include "qtwain_p.h"

using namespace QtTWAIN;

QTWAINScanner::QTWAINScanner(QTWAINScannerPrivate *d) : dPtr(d)
{
    dPtr->qPtr = this;
}

quint32 QTWAINScanner::id() const
{
    return dPtr->dataSource.Id;
}

quint16 QTWAINScanner::versionMajor() const
{
    return dPtr->dataSource.Version.MajorNum;
}

quint16 QTWAINScanner::versionMinor() const
{
    return dPtr->dataSource.Version.MinorNum;
}

quint16 QTWAINScanner::protocolMajor() const
{
    return dPtr->dataSource.ProtocolMajor;
}

quint16 QTWAINScanner::protocolMinor() const
{
    return dPtr->dataSource.ProtocolMinor;
}

QString QTWAINScanner::manufacturer() const
{
    return QString(dPtr->dataSource.Manufacturer);
}

QString QTWAINScanner::family() const
{
    return QString(dPtr->dataSource.ProductFamily);
}

QString QTWAINScanner::name() const
{
    return QString(dPtr->dataSource.ProductName);
}

QtTWAIN::Status QTWAINScanner::status() const
{
    return dPtr->status;
}

bool QTWAINScanner::scan()
{
    return dPtr->scan();
}

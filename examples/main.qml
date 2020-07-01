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

import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    anchors.fill: parent
    visible: true
    width: 640
    height: 480

    Row {
        id: toolBar
        spacing: 10
        Text {
            id: scannerName
            text: "Scanner: " + scanner.scannerName
        }
        Button {
            text: "Scan"
            onClicked: {
                scanner.scan();
            }
        }
    }

    Flickable {
        clip: true
        anchors {
            top: toolBar.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        contentWidth: preview.width
        contentHeight: preview.height
        Image {
            id: preview
            width: implicitWidth
            height: implicitHeight
            source: scanner.scannedImage != "" ? "data:image/png;base64," + scanner.scannedImage : ""
        }
    }
}

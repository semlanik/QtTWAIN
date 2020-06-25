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

#include "qtwaincontext_p.h"

#include "qtwainscanner.h"
#include "qtwainscanner_p.h"

#include <QDebug>

using namespace QtTWAIN;

QTWAINContext::QTWAINContext() : m_dsm(QDSMInterface::instance())
  , m_state(Initial)
  , m_windowHandle(nullptr)
{
    //TODO: refactor app identity initialization to real app name read from QApplication
    //Ok for now
    memset(&m_appIdentity, 0, sizeof(m_appIdentity));
    m_appIdentity.Version.MajorNum = 0;
    m_appIdentity.Version.MinorNum = 1;
    m_appIdentity.Version.Language = TWLG_ENGLISH_USA;
    m_appIdentity.Version.Country = TWCY_USA;
    memcpy(m_appIdentity.Version.Info, "0.1", sizeof(m_appIdentity.Version.Info) > strlen("0.1") ? strlen("0.1") : sizeof(m_appIdentity.Version.Info));
    m_appIdentity.ProtocolMajor = TWON_PROTOCOLMAJOR;
    m_appIdentity.ProtocolMinor = TWON_PROTOCOLMINOR;
    m_appIdentity.SupportedGroups = DF_APP2 | DG_IMAGE | DG_CONTROL;
    memcpy(m_appIdentity.Manufacturer, "QtTWAIN", sizeof(m_appIdentity.Manufacturer) > strlen("QtTWAIN") ? strlen("QtTWAIN") : sizeof(m_appIdentity.Manufacturer));
    memcpy(m_appIdentity.ProductFamily, "QtTWAIN", sizeof(m_appIdentity.ProductFamily) > strlen("QtTWAIN") ? strlen("QtTWAIN") : sizeof(m_appIdentity.ProductFamily));
    memcpy(m_appIdentity.ProductName, "QtTWAIN", sizeof(m_appIdentity.ProductName) > strlen("QtTWAIN") ? strlen("QtTWAIN") : sizeof(m_appIdentity.ProductName));

    memset(&m_status, 0, sizeof(m_status));
}

QTWAINContext::~QTWAINContext()
{
    closeDSM();
}


void QTWAINContext::openDSM(const WId winId)
{
    if (m_state != Initial) {
        qDebug() << "DSM is already opened. Skip.";
        return;
    }

    m_windowHandle = reinterpret_cast<TW_HANDLE>(winId);
    if (m_windowHandle != nullptr) {
        if (TWRC_SUCCESS != m_dsm->entry(&m_appIdentity, nullptr, DG_CONTROL, DAT_PARENT, MSG_OPENDSM, &m_windowHandle)) {
            QtTWAIN::QDSMInterface::instance()->entry(&m_appIdentity, nullptr, DG_CONTROL, DAT_STATUS, MSG_GET, &m_status);
            qCritical() << "Unable to open DSM: " << m_status.ConditionCode;
            return;
        }
        m_state = DSMOpen;
    } else {
        qCritical() << "Unable to open without active window on screen";
    }
}

void QTWAINContext::closeDSM()
{
    if (m_state != DSMOpen) {
        qDebug() << "DSM is incorrect state" << m_state << ", unable to close";
        return;
    }

    if (TWRC_SUCCESS == m_dsm->entry(&m_appIdentity, nullptr, DG_CONTROL, DAT_PARENT, MSG_CLOSEDSM, &m_windowHandle)) {
        m_windowHandle = NULL;
    }
}

void QTWAINContext::fetchScannerList()
{
    m_scannerList.clear();
    if (m_state != DSMOpen) {
        qCritical() << "Unable to fetch scanner list, DSM is incorrect state";
        return;
    }

    TW_IDENTITY dataSource;
    memset(&dataSource, 0, sizeof(TW_IDENTITY));

    if (TWRC_SUCCESS == m_dsm->entry(&m_appIdentity, nullptr, DG_CONTROL, DAT_IDENTITY, MSG_GETDEFAULT, &dataSource)) {
        qDebug() << "Default data source: " << dataSource.ProductName;
    }

    //Populate with all the connected data sources
    auto scanner_p = new QTWAINScannerPrivate(this);
    int result = m_dsm->entry(&m_appIdentity, nullptr, DG_CONTROL, DAT_IDENTITY, MSG_GETFIRST, &scanner_p->dataSource);
    do {
        qDebug() << "Found scanner: " << scanner_p->dataSource.ProductName;
        if (result == TWRC_SUCCESS) {
            m_scannerList[QString(m_appIdentity.ProductName)] = std::shared_ptr<QTWAINScanner>(new QTWAINScanner(scanner_p));
        } else {
            updateStatus();
            qDebug() << "Unable to add scanner: " << m_status.ConditionCode;
            delete scanner_p;
        }
        scanner_p = new QTWAINScannerPrivate(this);
        result = m_dsm->entry(&m_appIdentity, nullptr, DG_CONTROL, DAT_IDENTITY, MSG_GETNEXT, &(scanner_p->dataSource));
    } while(result == TWRC_SUCCESS);
}

bool QTWAINContext::openDS(TW_IDENTITY *dataSource)
{
    if (TWRC_SUCCESS != m_dsm->entry(&m_appIdentity, nullptr, DG_CONTROL, DAT_IDENTITY, MSG_OPENDS, dataSource)) {
        updateStatus();
        qDebug() << "Unable to open scanner: " << m_status.ConditionCode;
        return false;
    }

    return true;
}

void QTWAINContext::closeDS(TW_IDENTITY *dataSource)
{
    if (TWRC_SUCCESS != m_dsm->entry(&m_appIdentity, nullptr, DG_CONTROL, DAT_IDENTITY, MSG_CLOSEDS, dataSource)) {
        updateStatus();
        qDebug() << "Unable to close scanner: " << m_status.ConditionCode;
    }
}

bool QTWAINContext::enableDS(TW_IDENTITY *dataSource)
{
    TW_USERINTERFACE uiSettings;
    memset(&uiSettings, 0, sizeof(uiSettings));
    uiSettings.ShowUI = true;
    uiSettings.hParent = m_windowHandle;
    if (TWRC_SUCCESS != m_dsm->entry(&m_appIdentity, dataSource, DG_CONTROL, DAT_USERINTERFACE, MSG_ENABLEDS, &uiSettings)) {
        updateStatus();
        qDebug() << "Unable to enable scanner: " << m_status.ConditionCode;
        return false;
    }
    return true;
}

bool QTWAINContext::disableDS(TW_IDENTITY *dataSource)
{
    TW_USERINTERFACE uiSettings;
    memset(&uiSettings, 0, sizeof(uiSettings));
    if (TWRC_SUCCESS != m_dsm->entry(&m_appIdentity, dataSource, DG_CONTROL, DAT_USERINTERFACE, MSG_DISABLEDS, &uiSettings)) {
        updateStatus();
        qDebug() << "Unable to disable scanner: " << m_status.ConditionCode;
        return false;
    }

    return true;
}


bool QTWAINContext::processEvent(TW_EVENT *event, TW_IDENTITY *dataSource)
{
    return TWRC_DSEVENT == m_dsm->entry(&m_appIdentity, dataSource, DG_CONTROL, DAT_EVENT, MSG_PROCESSEVENT, event);
}

bool QTWAINContext::imageInfo(TW_IMAGEINFO *info, TW_IDENTITY *dataSource)
{
    if (TWRC_SUCCESS != m_dsm->entry(&m_appIdentity, dataSource, DG_IMAGE, DAT_IMAGEINFO, MSG_GET, info)) {
        updateStatus();
        qDebug() << "Unable to get information about image: " << m_status.ConditionCode;
        return false;
    }
    return true;
}

quint16 QTWAINContext::imageNativeTransfer(TW_HANDLE *imgHandle, TW_IDENTITY *dataSource)
{
    return m_dsm->entry(&m_appIdentity, dataSource, DG_IMAGE, DAT_IMAGENATIVEXFER, MSG_GET, imgHandle);
}

bool QTWAINContext::endTransfer(TW_PENDINGXFERS *pendingTransfers, TW_IDENTITY *dataSource)
{
    if (TWRC_SUCCESS != m_dsm->entry(&m_appIdentity, dataSource, DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, pendingTransfers)) {
        updateStatus();
        qDebug() << "Unable to complete image transfer: " << m_status.ConditionCode;
        return false;
    }
    return true;
}

void QTWAINContext::updateStatus(TW_IDENTITY *dataSource)
{
    QtTWAIN::QDSMInterface::instance()->entry(&m_appIdentity, dataSource, DG_CONTROL, DAT_STATUS, MSG_GET, &m_status);
}

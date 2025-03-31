/*
 *  Copyright (C) 2025 Bruce Anderson <bcom@andtru.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */

#include <QApplication>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <QPixmap>
#include <QTime>

#include "trayitem.h"

TrayItem::TrayItem(DockerDaemon *parent, int slotIndex, QString windowName, QString windowTitle, ConfigSettings *configFile) :
    m_configFile(configFile), m_parent(parent)
{
    m_customIcon = false;
    m_slotIndex = slotIndex;

    m_dockedAppName = windowName;
    m_windowTitle = windowTitle;

    updateTitle();
//     updateIcon();

    createContextMenu();
    this->show();

    connect(this, &QSystemTrayIcon::activated, this, &TrayItem::toggleWindow);

    QString path;
    configFile->getConfigItem(CUSTOM_ICON_KEY, path);
    setCustomIcon(path);
}

TrayItem::~TrayItem()
{
    // Only the main menu needs to be deleted. The rest of the menus and actions
    // are children of this menu and Qt will delete all children.
    delete m_contextMenu;
}

#if 0
TrayItemConfig TrayItem::readConfigGlobals()
{
    TrayItemConfig config;

    m_config.beginGroup("_GLOBAL_DEFAULTS");
      config.sCustomIcon           = m_config.value("CustomIcon",       DEFAULT_CustomIcon).toString();
      config.iBalloonTimeout       = m_config.value("BalloonTimeout",   DEFAULT_BalloonTimeout).toInt();
      config.opt[Sticky]           = m_config.value("Sticky",           DEFAULT_Sticky).toBool();
      config.opt[SkipPager]        = m_config.value("SkipPager",        DEFAULT_SkipPager).toBool();
      config.opt[SkipTaskbar]      = m_config.value("SkipTaskbar",      DEFAULT_SkipTaskbar).toBool();
      config.opt[IconifyMinimized] = m_config.value("IconifyMinimized", DEFAULT_IconifyMinimized).toBool();
      config.opt[IconifyObscured]  = m_config.value("IconifyObscured",  DEFAULT_IconifyObscured).toBool();
      config.opt[IconifyFocusLost] = m_config.value("IconifyFocusLost", DEFAULT_IconifyFocusLost).toBool();
      config.opt[LockToDesktop]    = m_config.value("LockToDesktop",    DEFAULT_LockToDesktop).toBool();
    m_config.endGroup();

    return config;
}
#endif

void TrayItem::saveSettingsGlobal()
{
    m_configFile->saveSettingsGlobal();
}

void TrayItem::saveSettingsApp()
{
    m_configFile->saveSettingsApp();
}

void TrayItem::closeWindow()
{
    m_parent->closeWindow(m_slotIndex);
}


void TrayItem::setCustomIcon(QString path)
{
    m_customIcon = true;
    QPixmap customIcon;
    if (customIcon.load(path)) {
        m_settings.sCustomIcon = path;
    } else {
        customIcon.load(":/images/question.png");
    }

    setIcon(QIcon(customIcon));
}

void TrayItem::selectCustomIcon(bool value)
{
    Q_UNUSED(value);

    QStringList types;
    QString supportedTypes;
    QString path;

    Q_FOREACH(QByteArray type, QImageReader::supportedImageFormats()) {
        types << QString(type);
    }
    if (types.isEmpty()) {
        supportedTypes = "All Files (*.*)";
    } else {
        supportedTypes = QString("Images (*.%1);;All Files (*.*)").arg(types.join(" *."));
    }

    path = QFileDialog::getOpenFileName(0, tr("Select Icon"), QDir::homePath(), supportedTypes);

    if (!path.isEmpty()) {
        setCustomIcon(path);
        m_configFile->setConfigItem(CUSTOM_ICON_KEY, path);
    }
}

void TrayItem::setSkipTaskbar(bool value)
{
    m_actionSkipTaskbar->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(SKIP_TASKBAR_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setSkipPager(bool value)
{
    m_actionSkipPager->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(SKIP_PAGER_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setSticky(bool value)
{
    m_actionSticky->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(STICKY_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setIconifyMinimized(bool value)
{
    m_actionIconifyMinimized->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(ICONIFY_IF_MINIMIZED_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setLockToDesktop(bool value)
{
    m_actionLockToDesktop->setChecked(value);
    m_configFile->setConfigItem(LOCK_TO_DESKTOP_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

#if 0
void TrayItem::setBalloonTimeout(int value)
{
    if (value < 0) {
        value = 0;
    }
    m_settings.iBalloonTimeout = value;
    m_actionBalloonTitleChanges->setChecked(value ? true : false);
}
#endif

void TrayItem::setBalloonOnTitleChange(bool value)
{
    m_actionBalloonOnTitleChange->setChecked(value);
    m_configFile->setConfigItem(BALLOON_TITLE_CHANGE_KEY, value);
    // Don't need to update parent configuration because
    // this only affects the tray icon
}

void TrayItem::toggleWindow()
{
    m_parent->toggleHideShow(m_slotIndex);
}

void TrayItem::doUndock()
{
    m_parent->doUndock(m_slotIndex);
}

void TrayItem::undockAll()
{
    m_parent->doUndockAll();
}

/*
 * Update the title in the tooltip.
 */
void TrayItem::updateTitle()
{
    setToolTip(QString("%1 [%2]").arg(m_windowTitle).arg(m_dockedAppName));
    bool balloonOnTitleChange;
    m_configFile->getConfigItem(BALLOON_TITLE_CHANGE_KEY, balloonOnTitleChange);
    if (balloonOnTitleChange) {
        printf("balloon title\n");
        showMessage(m_dockedAppName, m_windowTitle, QSystemTrayIcon::Information, 4000);
    }
}

void TrayItem::createContextMenu()
{
    bool boolVal;
    int intVal;
    QString stringVal;

    m_contextMenu = new QMenu();

    m_contextMenu->addAction(QIcon(":/images/about.png"), tr("About %1").arg(qApp->applicationName()), this, SIGNAL(about()));
    m_contextMenu->addSeparator();

    // Options menu
    m_optionsMenu = new QMenu(tr("Options"), m_contextMenu);
    m_optionsMenu-> setIcon(QIcon(":/images/options.png"));

    m_actionSetIcon = new QAction(tr("Set icon..."), m_optionsMenu);
    connect(m_actionSetIcon, SIGNAL(triggered(bool)), this, SLOT(selectCustomIcon(bool)));
    m_optionsMenu->addAction(m_actionSetIcon);

    m_actionSkipTaskbar = new QAction(tr("Skip taskbar"), m_optionsMenu);
    m_actionSkipTaskbar->setCheckable(true);
    m_configFile->getConfigItem(SKIP_TASKBAR_KEY, boolVal);
    m_actionSkipTaskbar->setChecked(boolVal);
    connect(m_actionSkipTaskbar, SIGNAL(triggered(bool)), this, SLOT(setSkipTaskbar(bool)));
    m_optionsMenu->addAction(m_actionSkipTaskbar);

    m_actionSkipPager = new QAction(tr("Skip pager"), m_optionsMenu);
    m_actionSkipPager->setCheckable(true);
    m_configFile->getConfigItem(SKIP_PAGER_KEY, boolVal);
    m_actionSkipPager->setChecked(boolVal);
    connect(m_actionSkipPager, SIGNAL(triggered(bool)), this, SLOT(setSkipPager(bool)));
    m_optionsMenu->addAction(m_actionSkipPager);

    m_actionSticky = new QAction(tr("Sticky"), m_optionsMenu);
    m_actionSticky->setCheckable(true);
    m_configFile->getConfigItem(STICKY_KEY, boolVal);
    m_actionSticky->setChecked(boolVal);
    connect(m_actionSticky, SIGNAL(triggered(bool)), this, SLOT(setSticky(bool)));
    m_optionsMenu->addAction(m_actionSticky);

    m_actionIconifyMinimized = new QAction(tr("Iconify when minimized"), m_optionsMenu);
    m_actionIconifyMinimized->setCheckable(true);
    m_configFile->getConfigItem(ICONIFY_IF_MINIMIZED_KEY, boolVal);
    m_actionIconifyMinimized->setChecked(boolVal);
    connect(m_actionIconifyMinimized, SIGNAL(triggered(bool)), this, SLOT(setIconifyMinimized(bool)));
    m_optionsMenu->addAction(m_actionIconifyMinimized);

    m_actionLockToDesktop = new QAction(tr("Lock to desktop"), m_optionsMenu);
    m_actionLockToDesktop->setCheckable(true);
    m_configFile->getConfigItem(LOCK_TO_DESKTOP_KEY, boolVal);
    m_actionLockToDesktop->setChecked(boolVal);
    connect(m_actionLockToDesktop, SIGNAL(toggled(bool)), this, SLOT(setLockToDesktop(bool)));
    m_optionsMenu->addAction(m_actionLockToDesktop);

    m_actionBalloonOnTitleChange = new QAction(tr("Balloon on title changes"), m_optionsMenu);
    m_actionBalloonOnTitleChange->setCheckable(true);
    m_configFile->getConfigItem(BALLOON_TITLE_CHANGE_KEY, boolVal);
    m_actionBalloonOnTitleChange->setChecked(boolVal);
    connect(m_actionBalloonOnTitleChange, SIGNAL(triggered(bool)), this, SLOT(setBalloonOnTitleChange(bool)));
    m_optionsMenu->addAction(m_actionBalloonOnTitleChange);

    m_contextMenu->addMenu(m_optionsMenu);

    // Save settings menu
    m_optionsMenu->addSeparator();
    m_defaultsMenu = new QMenu(tr("Save settings"), m_optionsMenu);
    m_defaultsMenu-> setIcon(QIcon(":/images/config.png"));

    m_actionSaveSettingsApp = new QAction(tr("%1 only").arg(m_dockedAppName), m_defaultsMenu);
    connect(m_actionSaveSettingsApp, SIGNAL(triggered()), this, SLOT(saveSettingsApp()));
    m_defaultsMenu->addAction(m_actionSaveSettingsApp);

    m_actionSaveSettingsGlobal = new QAction(tr("Global (all new)"), m_defaultsMenu);
    connect(m_actionSaveSettingsGlobal, SIGNAL(triggered()), this, SLOT(saveSettingsGlobal()));
    m_defaultsMenu->addAction(m_actionSaveSettingsGlobal);
    connect(m_actionSaveSettingsApp, SIGNAL(triggered()), this, SLOT(saveSettingsApp()));

    m_optionsMenu->addMenu(m_defaultsMenu);

    m_contextMenu->addAction(tr("Undock All"), this, SLOT(undockAll()));
    m_contextMenu->addSeparator();
//    m_actionToggle = new QAction(tr("Toggle"), m_contextMenu);
//    connect(m_actionToggle, SIGNAL(triggered()), this, SLOT(toggleWindow()));
//    m_contextMenu->addAction(m_actionToggle);
    m_contextMenu->addAction(tr("Undock"), this, SLOT(doUndock()));
    m_contextMenu->addAction(QIcon(":/images/close.png"), tr("Close"), this, SLOT(closeWindow()));

    setContextMenu(m_contextMenu);
}

void TrayItem::changeWindowTitle(QString newTitle)
{
    m_windowTitle = newTitle;
    updateTitle();
}


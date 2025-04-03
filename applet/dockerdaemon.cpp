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

/*
 * The DockerDaemon class is the heart of the WKDocker system which also
 * includes the kwin script that talks to the kwin compositor and the
 * TrayItem class which handles the SystemTray icon input and output,
 * one instance for each window (up to 10) docked to the SystemTray.
 *
 * The DockerDaemon will typically be spawned at login by autostart
 * or systemd. It then waits for notification from the kwin script
 * that the user has requested that a window be docked. It then
 * spawns a TrayItem instance which creates the actual icon.
 */

#include "dockerdaemon.h"
#include "trayitem.h"
#include "constants.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QApplication>

#define SCRIPT_SERVICE_NAME "org.kde.kglobalaccel"

/* Constructor */
DockerDaemon::DockerDaemon(QWidget *parent)
    : m_iface(QDBusInterface(SCRIPT_SERVICE_NAME, "/component/kwin"))
{
    // Setup the DBus connection interface
    auto connection = QDBusConnection::sessionBus();

    // Create a main window. Qt has a flag that allows the last
    // regular window to be deleted and still keep running
    // but this doesn't work for QSystemTrayIcon's. If you
    // delete the last one of these, the main program (i.e.
    // this daemon) shuts down which is not what we want
    // because we want to be able to dock a window, undock
    // the window, and continue on to be able to dock other
    // windows later. To get around this a QMainWindow
    // instance is created which has only one purpose -
    // to be "shown" just before TrayItem is deleted
    // and "hidden" again immediately thereafter. Hopefully
    // the user will never even see the flash of the
    // window being shown but it allows the daemon to
    // keep functioning.
    m_junk = new(QMainWindow);

    if (!connection.isConnected()) {
        qWarning("Cannot connect to the D-Bus session bus.\n"
                 "To start it, run:\n"
                 "\teval `dbus-launch --auto-syntax`\n");
        return;
    }
    for (int i=0; i<NUM_SLOTS; i++) {
        m_dockedWindows[i] = NULL;
    }

    // register DBus object at org.kde.myapp/foobar
    QDBusConnection::sessionBus().registerService("org.andtru.wkdocker");
    QDBusConnection::sessionBus().registerObject("/docker", this, QDBusConnection::ExportScriptableContents);
}

// Deconstructor
DockerDaemon::~DockerDaemon()
{
    QDBusConnection::sessionBus().unregisterObject("/docker");
    QDBusConnection::sessionBus().unregisterService("org.andtru.wkdocker");
}

/*
 * addNewWindow
 * In response to a DBus call from the kwin script, setup the applet configuration
 * from saved configuration file and/or system defaults and spawn an instance of
 * TrayItem to provide the SystemTray icon and user I/O. Then update the configuration
 * to the kwin script so it knows how to react.
 */
void DockerDaemon::addNewWindow(int slotIndex, QString windowName, QString windowTitle)
{
    if (slotIndex == ALREADY_DOCKED) {
        QMessageBox::critical(0, qApp->applicationName(), "Window already docked");
        return;
    } else if (slotIndex == SLOTS_FULL) {
        QMessageBox::critical(0, qApp->applicationName(), "Maximum number of windows already docked");
        return;
    } else if (slotIndex == NOT_NORMAL_WINDOW) {
        QMessageBox::critical(0, qApp->applicationName(), "Selected window isn't a normal window");
        return;
    } else if (slotIndex < 0 || slotIndex >= NUM_SLOTS)
        return;

    DockedWindow *currentWindow = new DockedWindow;
    currentWindow->config = new ConfigSettings(m_configFile, windowName);
    currentWindow->item = new TrayItem(this, slotIndex, windowName, windowTitle, currentWindow->config);
    m_dockedWindows[slotIndex] = currentWindow;
    connect(currentWindow->item, SIGNAL(about()), this, SLOT(about()));
    connect(currentWindow->item, SIGNAL(updateConfiguration(int)), this, SLOT(updateConfiguration(int)));

    updateConfiguration(slotIndex);
}

// Handle an updateConfiguration call from the kwin script
void DockerDaemon::updateConfiguration(int slotIndex)
{
    if (m_iface.isValid()) {
        const CommandStruct c = {slotIndex, SetupAvailable};
        m_commandQueue.enqueue(c);
        m_iface.call("invokeShortcut", "dockerCommandAvailable");
        return;
    }
}

// Handle a requestCommand from the kwin script.
void DockerDaemon::requestCommand(int &slotIndex, int &command) {
    if (!m_commandQueue.isEmpty()) {
        struct CommandStruct commandStruct = m_commandQueue.dequeue();
        slotIndex = commandStruct.slotIndex;
        command = commandStruct.command;
    } else {
        printf("requestCommand: queue was empty\n");
    }
};

// Handle a request for setup configuration from the kwin script
void DockerDaemon::requestSetup(int slotIndexIn,
                                int &slotIndexOut,
                                bool &skipPager,
                                bool &skipTaskbar,
                                bool &iconifyIfMinimized,
                                bool &lockToDeskTop,
                                bool &sticky)
{
    slotIndexOut = slotIndexIn;
    ConfigSettings *currentConfig = m_dockedWindows[slotIndexIn]->config;

    currentConfig->getConfigItem(SKIP_PAGER_KEY, skipPager);
    currentConfig->getConfigItem(SKIP_TASKBAR_KEY, skipTaskbar);
    currentConfig->getConfigItem(ICONIFY_IF_MINIMIZED_KEY, iconifyIfMinimized);
    currentConfig->getConfigItem(LOCK_TO_DESKTOP_KEY, lockToDeskTop);
    currentConfig->getConfigItem(STICKY_KEY, sticky);
}

// Possible future enhancement - In case we need to keep track for some reason in the future
void DockerDaemon::onManualMinimizeChange(int slotIndex, bool minimized)
{
}

/*
 * onClientClosed
 * Shut down and delete artifacts for a window when the user
 * closes it
 */
void DockerDaemon::onClientClosed(int slotIndex)
{
    DockedWindow *currentWindow = m_dockedWindows[slotIndex];

    if (currentWindow == NULL)
        return;
    delete currentWindow->config;
    m_junk->show();
    delete currentWindow->item;
    m_junk->hide();
    delete currentWindow;
    m_dockedWindows[slotIndex] = NULL;
}

/*
 * onCaptionChanged
 * Notify the TrayItem instance when the caption (window title)
 * changes, so it can update the tooltip and possibly put up
 * a temporary notification window (if the 'Balloon on Title Changes'
 * flag is set.
 */
void DockerDaemon::onCaptionChanged(int slotIndex, QString newTitle)
{
    m_dockedWindows[slotIndex]->item->changeWindowTitle(newTitle);
}

/*
 * toggleHideShow
 * Notify the kwin script to iconify or activate
 * the window (depending on whether it is currently
 * active or not.
 */
void DockerDaemon::toggleHideShow(int slotIndex)
{
    m_commandQueue.enqueue( {slotIndex, ToggleWindowState});
    m_iface.call("invokeShortcut", "dockerCommandAvailable");
}

/*
 * doUndock
 * Clean up config data, remove the TrayItem instance, and
 * notify the kwin script when the user requests that the
 * window be undocked.
 */
void DockerDaemon::doUndock(int slotIndex)
{
    DockedWindow *currentWindow = m_dockedWindows[slotIndex];
    if (currentWindow != NULL) {
        m_commandQueue.enqueue( {slotIndex, UndockWindow});
        m_iface.call("invokeShortcut", "dockerCommandAvailable");

        delete currentWindow->config;
        m_junk->show();
        delete currentWindow->item;
        m_junk->hide();
        delete currentWindow;
        m_dockedWindows[slotIndex] = NULL;
    }
}

void DockerDaemon::doUndockAll()
{
    for (int i; i < NUM_SLOTS; i++) {
        if (m_dockedWindows[i] != NULL) {
            doUndock(i);
        }
    }
}

/*
 * closeWindow
 * Notify the kwin script that the user wants
 * to close the window.
 */
void DockerDaemon::closeWindow(int slotIndex)
{
    m_commandQueue.enqueue({slotIndex, CloseWindow});
    m_iface.call("invokeShortcut", "dockerCommandAvailable");
}

void DockerDaemon::about()
{
    QMessageBox aboutBox;

    aboutBox.setIconPixmap(QPixmap(":/images/kdocker.png"));
    aboutBox.setWindowTitle(tr("About %1 - %2").arg(qApp->applicationName()).arg(qApp->applicationVersion()));
    aboutBox.setText(Constants::ABOUT_MESSAGE);
    aboutBox.setInformativeText(tr("See %1 for more information.").arg("<a href=\"https://github.com/user-none/KDocker\">https://github.com/user-none/KDocker</a>"));

    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.exec();
}


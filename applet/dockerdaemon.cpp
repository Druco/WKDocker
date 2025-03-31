#include "dockerdaemon.h"
#include "trayitem.h"
#include "constants.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QApplication>

#define SCRIPT_SERVICE_NAME "org.kde.kglobalaccel"

extern QApplication* gApp;

DockerDaemon::DockerDaemon(QWidget *parent)
    : m_iface(QDBusInterface(SCRIPT_SERVICE_NAME, "/component/kwin"))
{
    // Setup the DBus connection interface
    auto connection = QDBusConnection::sessionBus();

    // Create a main window but don't "show" it just so the
    // program won't shut down when the last tray item
    // goes away
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
    QDBusConnection::sessionBus().registerService("org.andtru.menutest");
    QDBusConnection::sessionBus().registerObject("/docker", this, QDBusConnection::ExportScriptableContents);
}

DockerDaemon::~DockerDaemon()
{
    QDBusConnection::sessionBus().unregisterObject("/docker");
    QDBusConnection::sessionBus().unregisterService("org.andtru.menutest");
}

void DockerDaemon::addNewWindow(int slotIndex, QString windowName, QString windowTitle)
{
    if (slotIndex == ALREADY_DOCKED) {
        QMessageBox::critical(0, gApp->applicationName(), "Window already docked");
        return;
    } else if (slotIndex == SLOTS_FULL) {
        QMessageBox::critical(0, gApp->applicationName(), "Maximum number of windows already docked");
        return;
    } else if (slotIndex == NOT_NORMAL_WINDOW) {
        QMessageBox::critical(0, gApp->applicationName(), "Selected window isn't a normal window");
        return;
    } else if (slotIndex < 0 || slotIndex >= NUM_SLOTS)
        return;

    DockedWindow* currentWindow = new DockedWindow;
    currentWindow->config = new ConfigSettings(m_configFile, windowName);
    currentWindow->item = new TrayItem(this, slotIndex, windowName, windowTitle, currentWindow->config);
    m_dockedWindows[slotIndex] = currentWindow;

    updateConfiguration(slotIndex);
}

void DockerDaemon::updateConfiguration(int slotIndex)
{
    if (m_iface.isValid()) {
        const CommandStruct c = {slotIndex, SetupAvailable};
        m_commandQueue.enqueue(c);
        m_iface.call("invokeShortcut", "dockerCommandAvailable");
        return;
    }
}

void DockerDaemon::requestCommand(int &slotIndex, int &command) {
    if (!m_commandQueue.isEmpty())
    {
        struct CommandStruct commandStruct = m_commandQueue.dequeue();
        slotIndex = commandStruct.slotIndex;
        command = commandStruct.command;
    }
    else
    {
        printf("requestCommand: queue was empty\n");
    }
};

void DockerDaemon::requestSetup(int slotIndexIn,
                                int &slotIndexOut,
                                bool &skipPager,
                                bool &skipTaskbar,
                                bool &iconifyIfMinimized,
                                bool &iconifyIfObscured,
                                bool &iconifyIfFocusLost,
                                bool &lockToDeskTop,
                                bool &sticky)
{
    slotIndexOut = slotIndexIn;
    ConfigSettings* currentConfig = m_dockedWindows[slotIndexIn]->config;

    currentConfig->getConfigItem(SKIP_PAGER_KEY, skipPager);
    currentConfig->getConfigItem(SKIP_TASKBAR_KEY, skipTaskbar);
    currentConfig->getConfigItem(ICONIFY_IF_MINIMIZED_KEY, iconifyIfMinimized);
    currentConfig->getConfigItem(ICONIFY_IF_OBSCURED_KEY, iconifyIfObscured);
    currentConfig->getConfigItem(ICONIFY_IF_OBSCURED_KEY, iconifyIfFocusLost);
    currentConfig->getConfigItem(LOCK_TO_DESKTOP_KEY, lockToDeskTop);
    currentConfig->getConfigItem(STICKY_KEY, sticky);
}

// BAA - In case we need to keep track for some reason in the future
void DockerDaemon::onManualMinimizeChange(int slotIndex, bool minimized) {
}

void DockerDaemon::onClientClosed(int slotIndex) {
    DockedWindow* currentWindow = m_dockedWindows[slotIndex];

    if (currentWindow == NULL)
        return;
    delete currentWindow->config;
    m_junk->show();
    delete currentWindow->item;
    m_junk->hide();
    delete currentWindow;
    m_dockedWindows[slotIndex] = NULL;
}

void DockerDaemon::onCaptionChanged(int slotIndex, QString newTitle) {
    m_dockedWindows[slotIndex]->item->changeWindowTitle(newTitle);
}


void DockerDaemon::toggleHideShow(int slotIndex) {
    m_commandQueue.enqueue( {slotIndex, ToggleWindowState});
    m_iface.call("invokeShortcut", "dockerCommandAvailable");
}

void DockerDaemon::doUndock(int slotIndex) {
    DockedWindow* currentWindow = m_dockedWindows[slotIndex];
    m_commandQueue.enqueue( {slotIndex, UndockWindow});
    m_iface.call("invokeShortcut", "dockerCommandAvailable");

    delete currentWindow->config;
    m_junk->show();
    delete currentWindow->item;
    m_junk->hide();
    delete currentWindow;
    m_dockedWindows[slotIndex] = NULL;
}

void DockerDaemon::doUndockAll() {
    for (int i; i < NUM_SLOTS; i++) {
        if (m_dockedWindows[i] != NULL) {
            doUndock(i);
        }
    }
}



void DockerDaemon::closeWindow(int slotIndex)
{
    m_commandQueue.enqueue({slotIndex, CloseWindow});
    m_iface.call("invokeShortcut", "dockerCommandAvailable");
}

#ifndef DOCKERDAEMON_H_INCLUDED
#define DOCKERDAEMON_H_INCLUDED

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QSettings>
#include <QDBusInterface>
#include <QMainWindow>

#include "constants.h"
#include "trayitem.h"
#include "configsettings.h"

class TrayItem;  // Forward reference

struct DockedWindow {
    TrayItem* item;
    ConfigSettings* config;
};

class DockerDaemon : public QObject
{
    Q_OBJECT

public:
    DockerDaemon(QWidget *parent = nullptr);
    ~DockerDaemon();
    void toggleHideShow(int slotIndex);

    QMainWindow* m_junk;

    Q_SCRIPTABLE void addNewWindow(int slotIndex, QString windowName);
    Q_SCRIPTABLE void requestSetup(int slotIndexIn,
                                  int &slotIndexOut,
                                  bool &skipPager,
                                  bool &skipTaskbar,
                                  bool &iconifyIfMinimized,
                                  bool &iconifyIfObscured,
                                  bool &iconfiyIfFocusLost,
                                  bool &lockToDeskTop,
                                  bool &sticky);

    Q_SCRIPTABLE void onManualMinimizeChange(int slotIndex, bool minimized);
    Q_SCRIPTABLE void onClientClosed(int slotIndex);

    void updateConfiguration(int slotIndex);
    void doUndock(int slotIndex);
    void doUndockAll();
    void closeWindow(int slotIndex);

private:
    QSettings m_configFile;
    QDBusInterface m_iface;
    const char* getSlotCommand(int slotIndex);
    DockedWindow* m_dockedWindows[NUM_SLOTS];

};
#endif // DOCKERDAEMON_H_INCLUDED

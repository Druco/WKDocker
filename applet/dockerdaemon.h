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

#ifndef DOCKERDAEMON_H
#define DOCKERDAEMON_H

#include "constants.h"
#include "trayitem.h"
#include "configsettings.h"

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QSettings>
#include <QDBusInterface>
#include <QMainWindow>
#include <QQueue>

class TrayItem;  // Forward reference

struct DockedWindow
{
    TrayItem *item;
    ConfigSettings *config;
};

class DockerDaemon : public QObject
{
    Q_OBJECT

public:
    DockerDaemon(QWidget *parent = nullptr);
    ~DockerDaemon();

    Q_SCRIPTABLE void addNewWindow(int slotIndex, QString windowName, QString windowTitle);
    Q_SCRIPTABLE void requestSetup(int slotIndexIn,
                                  int &slotIndexOut,
                                  bool &skipPager,
                                  bool &skipTaskbar,
                                  bool &iconifyIfMinimized,
                                  bool &lockToDeskTop,
                                  bool &sticky);

    Q_SCRIPTABLE void onManualMinimizeChange(int slotIndex, bool minimized);
    Q_SCRIPTABLE void onClientClosed(int slotIndex);
    Q_SCRIPTABLE void onCaptionChanged(int slotIndex, QString newTitle);
    Q_SCRIPTABLE void requestCommand(int &slotIndex, int &command);


public slots:
    void about();
    void updateConfiguration(int slotIndex);
    void closeWindow(int slotIndex);
    void doUndockAll();
    void doUndock(int slotIndex);
    void toggleHideShow(int slotIndex);

private:
    QSettings m_configFile;
    QDBusInterface m_iface;
    const char *getSlotCommand(int slotIndex);
    DockedWindow *m_dockedWindows[NUM_SLOTS];

    /* The values in this enum must match the constants defined
       in the 'main.js' file of the kwin script.
    */
    enum Command {
        ToggleWindowState = 4001,
        UndockWindow,
        UndockAll,
        CloseWindow,
        SetupAvailable
    };

    struct CommandStruct
    {
        int slotIndex;
        Command command;
    };

    QQueue<CommandStruct> m_commandQueue;
};
#endif // DOCKERDAEMON_H

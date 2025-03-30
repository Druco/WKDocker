/*
 *  Copyright (C) 2009 John Schember <john@nachtimwald.com>
 *  Copyright (C) 2004 Girish Ramakrishnan All Rights Reserved.
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

#ifndef _TRAYITEM_H
#define	_TRAYITEM_H

#include <QAction>
#include <QEvent>
#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QSystemTrayIcon>
#include <QSettings>
#include "configsettings.h"
#include "dockerdaemon.h"

class DockerDaemon;

#if 0
#include "xlibutil.h"
#endif

#define DEFAULT_CustomIcon        QString()
#define DEFAULT_BalloonTimeout    4000       // 4 seconds
#define DEFAULT_SkipTaskbar       false
#define DEFAULT_SkipPager         false
#define DEFAULT_Sticky            false
#define DEFAULT_IconifyMinimized  true
#define DEFAULT_IconifyObscured   false
#define DEFAULT_IconifyFocusLost  false
#define DEFAULT_LockToDesktop     true       // Restore to original desktop (default) else restore to active desktop

#define NOARG (int8_t)-1    // 'bool' in unset state


enum Option
{
    SkipTaskbar,
    SkipPager,
    Sticky,
    IconifyMinimized,
    IconifyObscured,
    IconifyFocusLost,
    LockToDesktop,
    Option_MAX
};

struct TrayItemArgs {   // from cmd line only
    QString sCustomIcon;
    int iBalloonTimeout;
    int8_t opt[Option_MAX];
};

typedef TrayItemArgs TrayItemConfig;

struct TrayItemSettings {  // from 1) cmd line, 2) app config, 3) global config, 4) default
    QString sCustomIcon;
    int iBalloonTimeout;
    bool opt[Option_MAX];
};

class TrayItem : public QSystemTrayIcon {
    Q_OBJECT

public:
    TrayItem(DockerDaemon* parent, int slotNumber, QString windowName, QString windowTitle, ConfigSettings* configFile);
    ~TrayItem();

#if 0
    Window dockedWindow();

    // Pass on all events through this interface
    bool xcbEventFilter(xcb_generic_event_t *event, xcb_window_t dockedWindow);
#endif

    void showWindow();
    void restoreWindow();
    void iconifyWindow();

    void doSkipTaskbar();
    void doSkipPager();
    void doSticky();

public slots:
    void closeWindow();
    void hideOptionsMenu();  // BAA Added
    void setCustomIcon(QString path);
    void selectCustomIcon(bool value);
    void setSkipTaskbar(bool value);
    void setSkipPager(bool value);
    void setSticky(bool value);
    void setIconifyMinimized(bool value);
    void setIconifyObscured(bool value);
    void setIconifyFocusLost(bool value);
    void setLockToDesktop(bool value);
    void setBalloonTimeout(int value);
    void setBalloonTimeout(bool value);

private slots:
    void toggleWindow();
    void trayActivated(QSystemTrayIcon::ActivationReason reason = QSystemTrayIcon::Trigger);
    void doUndock();
    void undockAll();
//    void saveSettingsGlobal();
    void saveSettingsApp();

signals:
    void selectAnother();
    void dead(TrayItem*);
    void undock(TrayItem*);
    void about();

protected:
    bool event(QEvent *e);

private:
    //   readSetting overloaded function
    bool    readSetting(/* int8_t  argSetting,*/ QString key, bool    kdockerDefault);
    int     readSetting(/*int     argSetting,*/ QString key, int     kdockerDefault);
    QString readSetting(/*QString argSetting,*/ QString key, QString kdockerDefault);
    int  nonZeroBalloonTimeout();
    TrayItemConfig readConfigGlobals();
    void saveSettings();

    void minimizeEvent();
    void destroyEvent();
#if 0
    void propertyChangeEvent(Atom property);
#endif
    void obscureEvent();
    void focusLostEvent();

    void set_NET_WM_STATE(const char *type, bool set);

    void readDockedAppName();
    void updateTitle();
    void updateIcon();
    void updateToggleAction();

    void createContextMenu();
    QIcon createIcon(/* Window window */);

    bool isBadWindow();
    bool isOnCurrentDesktop();

    bool m_customIcon;

    DockerDaemon* m_parent;
    int m_slotIndex;
    ConfigSettings* m_configFile;
    TrayItemSettings m_settings;

#if 0
    // SizeHint of m_window
    XSizeHints m_sizeHint;
    // The window that is associated with the tray icon.
    Window m_window;
#endif
    long m_desktop;
    QString m_dockedAppName;
    QString m_windowTitle;

    QMenu *m_contextMenu;
    QMenu *m_optionsMenu;
    QAction *m_actionSetIcon;
    QAction *m_actionSkipTaskbar;
    QAction *m_actionSkipPager;
    QAction *m_actionSticky;
    QAction *m_actionIconifyMinimized;
    QAction *m_actionIconifyObscured;
    QAction *m_actionIconifyFocusLost;
    QAction *m_actionLockToDesktop;
    QAction *m_actionBalloonTitleChanges;
    QAction *m_actionToggle;
    QMenu *m_defaultsMenu;
    QAction *m_actionSaveSettingsApp;
    QAction *m_actionSaveSettingsGlobal;
};

#endif	/* _TRAYITEM_H */


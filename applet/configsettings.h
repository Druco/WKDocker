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

#ifndef CONFIGSETTINGS_H_INCLUDED_
#define CONFIGSETTINGS_H_INCLUDED_

#include <QSettings>
#include "constants.h"

#define CUSTOM_ICON_KEY           "CustomIcon"
#define BALLOON_TITLE_CHANGE_KEY  "BalloonTitle"
#define STICKY_KEY                "Sticky"
#define SKIP_PAGER_KEY            "SkipPager"
#define SKIP_TASKBAR_KEY          "SkipTaskbar"
#define ICONIFY_IF_MINIMIZED_KEY  "IconifyIfMinimized"
#define ICONIFY_IF_OBSCURED_KEY   "IconifyIfObscured"
#define ICONIFY_IF_FOCUS_LOST_KEY "IconifyIfFocusLost"
#define LOCK_TO_DESKTOP_KEY       "LockToDeskTop"

#define DEFAULT_CustomIcon          QString()
#define DEFAULT_BalloonTimeout      4000       // 4 seconds
#define DEFAULT_SkipTaskbar         false
#define DEFAULT_SkipPager           false
#define DEFAULT_Sticky              false
#define DEFAULT_IconifyIfMinimized  true
#define DEFAULT_IconifyIfObscured   false
#define DEFAULT_IconifyIfFocusLost  false
#define DEFAULT_LockToDesktop       true       // Restore to original desktop (default) else restore to active desktop
#define DEFAULT_BalloonOnTitleChange false


class ConfigSettings : public QObject
{
    Q_OBJECT
public:
    ConfigSettings(QSettings& storedConfig, QString windowName);
    void getConfigItem(QString key, int& val);
    void getConfigItem(QString key, bool& val);
    void getConfigItem(QString key, QString& val);
    void setConfigItem(QString key, int val);
    void setConfigItem(QString key, bool val);
    void setConfigItem(QString key, QString val);
    void saveSettingsGlobal();
    void saveSettingsApp();

private:
    QSettings& m_configFile;
    QString m_windowName;
    QString m_customIcon;
    bool m_sticky;
    bool m_skipTaskbar;
    bool m_skipPager;
    bool m_iconifyIfMinimized;
    bool m_iconifyIfObscured;
    bool m_iconifyIfFocusLost;
    bool m_lockToDesktop;
    bool m_balloonOnTitleChange;
};
#endif // CONFIGSETTINGS_H_INCLUDED_

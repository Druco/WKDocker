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

#include "configsettings.h"

ConfigSettings::ConfigSettings(QSettings& configFile, QString windowName) :
    m_configFile(configFile)
{
    m_windowName = windowName;

    m_configFile.beginGroup("_GLOBAL_DEFAULTS");
    m_customIcon           = m_configFile.value(CUSTOM_ICON_KEY,           DEFAULT_CustomIcon).toString();
    m_balloonOnTitleChange = m_configFile.value(BALLOON_TITLE_CHANGE_KEY,  DEFAULT_BalloonOnTitleChange).toBool();
    m_sticky               = m_configFile.value(STICKY_KEY,                DEFAULT_Sticky).toBool();
    m_skipPager            = m_configFile.value(SKIP_PAGER_KEY,            DEFAULT_SkipPager).toBool();
    m_skipTaskbar          = m_configFile.value(SKIP_TASKBAR_KEY,          DEFAULT_SkipTaskbar).toBool();
    m_iconifyIfMinimized   = m_configFile.value(ICONIFY_IF_MINIMIZED_KEY,  DEFAULT_IconifyIfMinimized).toBool();
    m_lockToDesktop        = m_configFile.value(LOCK_TO_DESKTOP_KEY,       DEFAULT_LockToDesktop).toBool();
    m_configFile.endGroup();

    m_configFile.beginGroup(m_windowName);
    m_customIcon           = m_configFile.value(CUSTOM_ICON_KEY,           m_customIcon).toString();
    m_balloonOnTitleChange = m_configFile.value(BALLOON_TITLE_CHANGE_KEY,  m_balloonOnTitleChange).toBool();
    m_sticky               = m_configFile.value(STICKY_KEY,                m_sticky).toBool();
    m_skipPager            = m_configFile.value(SKIP_PAGER_KEY,            m_skipPager).toBool();
    m_skipTaskbar          = m_configFile.value(SKIP_TASKBAR_KEY,          m_skipTaskbar).toBool();
    m_iconifyIfMinimized   = m_configFile.value(ICONIFY_IF_MINIMIZED_KEY,  m_iconifyIfMinimized).toBool();
    m_lockToDesktop        = m_configFile.value(LOCK_TO_DESKTOP_KEY,       m_lockToDesktop).toBool();
    m_configFile.endGroup();

};

void ConfigSettings::getConfigItem(QString key, bool& val)
{
    if (key == STICKY_KEY)
        val = m_sticky;
    else if (key == SKIP_PAGER_KEY)
        val = m_skipPager;
    else if (key == SKIP_TASKBAR_KEY)
        val = m_skipTaskbar;
    else if (key == ICONIFY_IF_MINIMIZED_KEY)
        val = m_iconifyIfMinimized;
    else if (key == LOCK_TO_DESKTOP_KEY)
        val = m_lockToDesktop;
    else if (key == BALLOON_TITLE_CHANGE_KEY)
        val = m_balloonOnTitleChange;
    else
        val = false;
}

void ConfigSettings::getConfigItem(QString key, QString& val)
{
    if (key == CUSTOM_ICON_KEY)
        val = m_customIcon;
}

void ConfigSettings::setConfigItem(QString key, bool val)
{
    if (key == STICKY_KEY)
        m_sticky = val;
    else if (key == SKIP_PAGER_KEY)
        m_skipPager = val;
    else if (key == SKIP_TASKBAR_KEY)
        m_skipTaskbar = val;
    else if (key == ICONIFY_IF_MINIMIZED_KEY)
        m_iconifyIfMinimized = val;
    else if (key == LOCK_TO_DESKTOP_KEY)
        m_lockToDesktop = val;
    else if (key == BALLOON_TITLE_CHANGE_KEY)
        m_balloonOnTitleChange = val;
}

void ConfigSettings::setConfigItem(QString key, QString val)
{
    if (key == CUSTOM_ICON_KEY)
        m_customIcon = val;
}

void ConfigSettings::saveSettingsGlobal()
{
    m_configFile.beginGroup("_GLOBAL_DEFAULTS");
    m_configFile.setValue(CUSTOM_ICON_KEY,    m_customIcon);
    m_configFile.setValue(STICKY_KEY,                m_sticky);
    m_configFile.setValue(SKIP_PAGER_KEY,            m_skipPager);
    m_configFile.setValue(SKIP_TASKBAR_KEY,          m_skipTaskbar);
    m_configFile.setValue(ICONIFY_IF_MINIMIZED_KEY,  m_iconifyIfMinimized);
    m_configFile.setValue(LOCK_TO_DESKTOP_KEY,       m_lockToDesktop);
    m_configFile.setValue(BALLOON_TITLE_CHANGE_KEY,  m_balloonOnTitleChange);
    m_configFile.endGroup();
}

void ConfigSettings::saveSettingsApp()
{
    m_configFile.beginGroup(m_windowName);
    m_configFile.setValue(CUSTOM_ICON_KEY,    m_customIcon);
    m_configFile.setValue(STICKY_KEY,                m_sticky);
    m_configFile.setValue(SKIP_PAGER_KEY,            m_skipPager);
    m_configFile.setValue(SKIP_TASKBAR_KEY,          m_skipTaskbar);
    m_configFile.setValue(ICONIFY_IF_MINIMIZED_KEY,  m_iconifyIfMinimized);
    m_configFile.setValue(LOCK_TO_DESKTOP_KEY,       m_lockToDesktop);
    m_configFile.setValue(BALLOON_TITLE_CHANGE_KEY,  m_balloonOnTitleChange);
    m_configFile.endGroup();
}


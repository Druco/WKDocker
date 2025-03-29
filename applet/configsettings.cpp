#include "configsettings.h"


ConfigSettings::ConfigSettings(QSettings& configFile, QString windowName) :
    m_configFile(configFile)
{
    m_windowName = windowName;

    m_configFile.beginGroup("_GLOBAL_DEFAULTS");
    m_customIcon           = m_configFile.value(CUSTOM_ICON_KEY,           DEFAULT_CustomIcon).toString();
    m_balloonTimeout       = m_configFile.value(BALLOON_TIMEOUT_KEY,       DEFAULT_BalloonTimeout).toInt();
    m_sticky               = m_configFile.value(STICKY_KEY,                DEFAULT_Sticky).toBool();
    m_skipPager            = m_configFile.value(SKIP_PAGER_KEY,            DEFAULT_SkipPager).toBool();
    m_skipTaskbar          = m_configFile.value(SKIP_TASKBAR_KEY,          DEFAULT_SkipTaskbar).toBool();
    m_iconifyIfMinimized   = m_configFile.value(ICONIFY_IF_MINIMIZED_KEY,  DEFAULT_IconifyIfMinimized).toBool();
    m_iconifyIfObscured    = m_configFile.value(ICONIFY_IF_OBSCURED_KEY,   DEFAULT_IconifyIfObscured).toBool();
    m_iconifyIfFocusLost   = m_configFile.value(ICONIFY_IF_FOCUS_LOST_KEY, DEFAULT_IconifyIfFocusLost).toBool();
    m_lockToDesktop        = m_configFile.value(LOCK_TO_DESKTOP_KEY,       DEFAULT_LockToDesktop).toBool();
    m_configFile.endGroup();

    m_configFile.beginGroup(m_windowName);
    m_customIcon           = m_configFile.value(CUSTOM_ICON_KEY,           m_customIcon).toString();
    m_balloonTimeout       = m_configFile.value(BALLOON_TIMEOUT_KEY,       m_balloonTimeout).toInt();
    m_sticky               = m_configFile.value(STICKY_KEY,                m_sticky).toBool();
    m_skipPager            = m_configFile.value(SKIP_PAGER_KEY,            m_skipPager).toBool();
    m_skipTaskbar          = m_configFile.value(SKIP_TASKBAR_KEY,          m_skipTaskbar).toBool();
    m_iconifyIfMinimized   = m_configFile.value(ICONIFY_IF_MINIMIZED_KEY,  m_iconifyIfMinimized).toBool();
    m_iconifyIfObscured    = m_configFile.value(ICONIFY_IF_OBSCURED_KEY,   m_iconifyIfObscured).toBool();
    m_iconifyIfFocusLost   = m_configFile.value(ICONIFY_IF_FOCUS_LOST_KEY, m_iconifyIfFocusLost).toBool();
    m_lockToDesktop        = m_configFile.value(LOCK_TO_DESKTOP_KEY,       m_lockToDesktop).toBool();
    m_configFile.endGroup();

};

void ConfigSettings::getConfigItem(QString key, int& val) {
    if (key == BALLOON_TIMEOUT_KEY)
        val = m_balloonTimeout;
}

void ConfigSettings::getConfigItem(QString key, bool& val) {
    if (key == STICKY_KEY)
        val = m_sticky;
    else if (key == SKIP_PAGER_KEY)
        val = m_skipPager;
    else if (key == SKIP_TASKBAR_KEY)
        val = m_skipTaskbar;
    else if (key == ICONIFY_IF_MINIMIZED_KEY)
        val = m_iconifyIfMinimized;
    else if (key == ICONIFY_IF_OBSCURED_KEY)
        val = m_iconifyIfObscured;
    else if (key == ICONIFY_IF_FOCUS_LOST_KEY)
        val = m_iconifyIfFocusLost;
    else if (key == LOCK_TO_DESKTOP_KEY)
        val = m_lockToDesktop;
}

void ConfigSettings::getConfigItem(QString key, QString& val) {
    if (key == CUSTOM_ICON_KEY)
        val = m_customIcon;
}

void ConfigSettings::setConfigItem(QString key, int val) {
    if (key == BALLOON_TIMEOUT_KEY)
        m_balloonTimeout = val;
}

void ConfigSettings::setConfigItem(QString key, bool val) {
    if (key == STICKY_KEY)
        m_sticky = val;
    else if (key == SKIP_PAGER_KEY)
        m_skipPager = val;
    else if (key == SKIP_TASKBAR_KEY)
        m_skipTaskbar = val;
    else if (key == ICONIFY_IF_MINIMIZED_KEY)
        m_iconifyIfMinimized = val;
    else if (key == ICONIFY_IF_OBSCURED_KEY)
        m_iconifyIfObscured = val;
    else if (key == ICONIFY_IF_FOCUS_LOST_KEY)
        m_iconifyIfFocusLost = val;
    else if (key == LOCK_TO_DESKTOP_KEY)
        m_lockToDesktop = val;
}

void ConfigSettings::setConfigItem(QString key, QString val) {
    if (key == CUSTOM_ICON_KEY)
        m_customIcon = val;
}

void ConfigSettings::saveSettingsGlobal() {
    m_configFile.beginGroup("_GLOBAL_DEFAULTS");
    m_configFile.setValue(CUSTOM_ICON_KEY,    m_customIcon);
    m_configFile.setValue(BALLOON_TIMEOUT_KEY,       m_balloonTimeout);
    m_configFile.setValue(STICKY_KEY,                m_sticky);
    m_configFile.setValue(SKIP_PAGER_KEY,            m_skipPager);
    m_configFile.setValue(SKIP_TASKBAR_KEY,          m_skipTaskbar);
    m_configFile.setValue(ICONIFY_IF_MINIMIZED_KEY,  m_iconifyIfMinimized);
    m_configFile.setValue(ICONIFY_IF_OBSCURED_KEY,   m_iconifyIfObscured);
    m_configFile.setValue(ICONIFY_IF_FOCUS_LOST_KEY, m_iconifyIfFocusLost);
    m_configFile.setValue(LOCK_TO_DESKTOP_KEY,       m_lockToDesktop);
    m_configFile.endGroup();
}

void ConfigSettings::saveSettingsApp() {
    m_configFile.beginGroup(m_windowName);
    m_configFile.setValue(CUSTOM_ICON_KEY,    m_customIcon);
    m_configFile.setValue(BALLOON_TIMEOUT_KEY,       m_balloonTimeout);
    m_configFile.setValue(STICKY_KEY,                m_sticky);
    m_configFile.setValue(SKIP_PAGER_KEY,            m_skipPager);
    m_configFile.setValue(SKIP_TASKBAR_KEY,          m_skipTaskbar);
    m_configFile.setValue(ICONIFY_IF_MINIMIZED_KEY,  m_iconifyIfMinimized);
    m_configFile.setValue(ICONIFY_IF_OBSCURED_KEY,   m_iconifyIfObscured);
    m_configFile.setValue(ICONIFY_IF_FOCUS_LOST_KEY, m_iconifyIfFocusLost);
    m_configFile.setValue(LOCK_TO_DESKTOP_KEY,       m_lockToDesktop);
    m_configFile.endGroup();
}


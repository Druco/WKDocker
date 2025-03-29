#ifndef CONFIGSETTINGS_H_INCLUDED_
#define CONFIGSETTINGS_H_INCLUDED_

#include <QSettings>
#include "constants.h"

#define CUSTOM_ICON_KEY           "CustomIcon"
#define BALLOON_TIMEOUT_KEY       "BalloonTimeout"
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
    int m_balloonTimeout;
    bool m_sticky;
    bool m_skipTaskbar;
    bool m_skipPager;
    bool m_iconifyIfMinimized;
    bool m_iconifyIfObscured;
    bool m_iconifyIfFocusLost;
    bool m_lockToDesktop;
};
#endif // CONFIGSETTINGS_H_INCLUDED_

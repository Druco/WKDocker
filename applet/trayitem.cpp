/*
 *  Copyright (C) 2009, 2015 John Schember <john@nachtimwald.com>
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

#include <QApplication>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <QPixmap>
#include <QTime>

#include "trayitem.h"

TrayItem::TrayItem(DockerDaemon* parent, int slotIndex, QString windowName, QString windowTitle, ConfigSettings* configFile) :
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

#if 0
//     updateToggleAction();

    if (!m_settings.sCustomIcon.isEmpty()) {
        setCustomIcon(m_settings.sCustomIcon);
    }
    setBalloonTimeout(m_settings.iBalloonTimeout);
    setSticky(m_settings.opt[Sticky]);
    setSkipPager(m_settings.opt[SkipPager]);
    setSkipTaskbar(m_settings.opt[SkipTaskbar]);
    setIconifyMinimized(m_settings.opt[IconifyMinimized]);
    setIconifyObscured(m_settings.opt[IconifyObscured]);
    setIconifyFocusLost(m_settings.opt[IconifyFocusLost]);
    setLockToDesktop(m_settings.opt[LockToDesktop]);

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
#endif
}

TrayItem::~TrayItem() {
    // Only the main menu needs to be deleted. The rest of the menus and actions
    // are children of this menu and Qt will delete all children.
    delete m_contextMenu;
}

bool TrayItem::readSetting(/*int8_t argSetting,*/ QString key, bool kdockerDefault) {
#if 0
    /* Precedence:
     * 1) Command line overrides         (argSetting, if positive)
     * 2) User app-specific defaults     (QSettings: "<m_dockedAppName>/<key>")
     * 3) User global defaults           (QSettings: "_GLOBAL_DEFAULTS/<key>")
     * 4) KDocker defaults               (#define DEFAULT_keyname)
     */
    if (argSetting != NOARG) {
        return (argSetting != 0);
    }
    key.prepend("%1/");   // Add formatting to local QString copy
    return m_config.value(key.arg(m_dockedAppName),
            m_config.value(key.arg("_GLOBAL_DEFAULTS"), kdockerDefault)).toBool();
#else
    return false;
#endif
}

int TrayItem::readSetting(/*int argSetting,*/ QString key, int kdockerDefault) {
#if 0
    if (argSetting >= 0) {
        return argSetting;
    }
    key.prepend("%1/");
    return m_config.value(key.arg(m_dockedAppName),
            m_config.value(key.arg("_GLOBAL_DEFAULTS"), kdockerDefault)).toInt();
#else
    return 1;
#endif
}

QString TrayItem::readSetting(/*QString argSetting,*/ QString key, QString kdockerDefault) {
#if 0
    if (!argSetting.isEmpty()) {
        return argSetting;
    }
    key.prepend("%1/");
    return m_config.value(key.arg(m_dockedAppName),
            m_config.value(key.arg("_GLOBAL_DEFAULTS"), kdockerDefault)).toString();
#else
    return "Placeholder";
#endif
}

int TrayItem::nonZeroBalloonTimeout() {
#if 0
    QString fmt = "%1/BalloonTimeout";
    int bto = m_config.value(fmt.arg(m_dockedAppName), 0).toInt();
    if (!bto) {
        bto = m_config.value(fmt.arg("_GLOBAL_DEFAULTS"), 0).toInt();
    }
    return bto ? bto : DEFAULT_BalloonTimeout;
#else
    return 1;
#endif
}

#if 0
TrayItemConfig TrayItem::readConfigGlobals() {
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

#if 0
void TrayItem::saveSettingsGlobal()
{
    m_config.beginGroup("_GLOBAL_DEFAULTS");
      saveSettings();
    m_config.endGroup();
}
#endif

void TrayItem::saveSettingsApp()
{
    m_configFile->saveSettingsApp();
#if 0
    TrayItemConfig globals = readConfigGlobals();

    m_config.beginGroup(m_dockedAppName);
      QVariant keyval;

      if (!m_settings.sCustomIcon.isEmpty()) {
          m_config.setValue("CustomIcon", m_settings.sCustomIcon);
      }
      saveSettings();

      // Remove app-specific settings if they match their default values

      keyval = m_config.value("BalloonTimeout");
      if (keyval.isValid() && (keyval.toInt()  == globals.iBalloonTimeout)) {
          m_config.remove("BalloonTimeout");
      }
      keyval = m_config.value("Sticky");
      if (keyval.isValid() && keyval.toBool() == globals.opt[Sticky]) {
          m_config.remove("Sticky");
      }
      keyval = m_config.value("SkipPager");
      if (keyval.isValid() && keyval.toBool() == globals.opt[SkipPager]) {
          m_config.remove("SkipPager");
      }
      keyval = m_config.value("SkipTaskbar");
      if (keyval.isValid() && keyval.toBool() == globals.opt[SkipTaskbar]) {
          m_config.remove("SkipTaskbar");
      }
      keyval = m_config.value("IconifyMinimized");
      if (keyval.isValid() && keyval.toBool() == globals.opt[IconifyMinimized]) {
          m_config.remove("IconifyMinimized");
      }
      keyval = m_config.value("IconifyObscured");
      if (keyval.isValid() && keyval.toBool() == globals.opt[IconifyObscured]) {
          m_config.remove("IconifyObscured");
      }
      keyval = m_config.value("IconifyFocusLost");
      if (keyval.isValid() && keyval.toBool() == globals.opt[IconifyFocusLost]) {
          m_config.remove("IconifyFocusLost");
      }
      keyval = m_config.value("LockToDesktop");
      if (keyval.isValid() && keyval.toBool() == globals.opt[LockToDesktop]) {
          m_config.remove("LockToDesktop");
      }
    m_config.endGroup();
#endif
}

void TrayItem::saveSettings() {    /*  "/home/<user>/.config/com.kdocker/KDocker.conf"    //  <==  m_config.fileName();  */
    // Group is set by caller
#if 0
    m_config.setValue("BalloonTimeout",   m_settings.iBalloonTimeout);
    m_config.setValue("Sticky",           m_settings.opt[Sticky]);
    m_config.setValue("SkipPager",        m_settings.opt[SkipPager]);
    m_config.setValue("SkipTaskbar",      m_settings.opt[SkipTaskbar]);
    m_config.setValue("IconifyMinimized", m_settings.opt[IconifyMinimized]);
    m_config.setValue("IconifyObscured",  m_settings.opt[IconifyObscured]);
    m_config.setValue("IconifyFocusLost", m_settings.opt[IconifyFocusLost]);
    m_config.setValue("LockToDesktop",    m_settings.opt[LockToDesktop]);
#endif
}

#if 0
Window TrayItem::dockedWindow() {
    return m_window;
}
#endif

void TrayItem::showWindow() {
#if 0
    show();

    if (m_settings.opt[IconifyMinimized]) {
        iconifyWindow();
    } else {
        if (m_settings.opt[SkipTaskbar]) {
            doSkipTaskbar();
        }
    }
#endif
}

void TrayItem::restoreWindow() {
#if 0
    if (isBadWindow()) {
        return;
    }

    m_is_restoring = true;

    Display *display = QX11Info::display();
    Window root = QX11Info::appRootWindow();

    if (m_iconified) {
        m_iconified = false;
        XMapWindow(display, m_window);
        m_sizeHint.flags = USPosition;
        XSetWMNormalHints(display, m_window, &m_sizeHint);

        updateToggleAction();
    }
    XMapRaised(display, m_window);
    XFlush(display);

    // Change to the desktop that the window was last on.
    long l_currDesk[2] = {m_desktop, CurrentTime};
    XLibUtil::sendMessage(display, root, root, "_NET_CURRENT_DESKTOP", 32, SubstructureNotifyMask | SubstructureRedirectMask, l_currDesk, sizeof (l_currDesk));

    if (m_settings.opt[LockToDesktop]) {
        // Set the desktop the window wants to be on.
        long l_wmDesk[2] = {m_desktop, 1}; // 1 == request sent from application. 2 == from pager
        XLibUtil::sendMessage(display, root, m_window, "_NET_WM_DESKTOP", 32, SubstructureNotifyMask | SubstructureRedirectMask, l_wmDesk, sizeof (l_wmDesk));
    }

    // Make it the active window
    // 1 == request sent from application. 2 == from pager.
    // We use 2 because KWin doesn't always give the window focus with 1.
    long l_active[2] = {2, CurrentTime};
    XLibUtil::sendMessage(display, root, m_window, "_NET_ACTIVE_WINDOW", 32, SubstructureNotifyMask | SubstructureRedirectMask, l_active, sizeof (l_active));
    XSetInputFocus(display, m_window, RevertToParent, CurrentTime);

    updateToggleAction();
    doSkipTaskbar();

    /*
     * Wait half a second to ensure the window is fully restored.
     * This and m_is_restoring are a work around for KWin.
     * KWin is the only WM that will send a PropertyNotify
     * event with the Iconic state set because of the above
     * XIconifyWindow call.
     */
    QTime t;
    t.start();
    while (t.elapsed() < 500) {
        qApp->processEvents();
    }

    m_is_restoring = false;
#endif
}

void TrayItem::iconifyWindow() {
#if 0
    if (isBadWindow() || m_is_restoring) {
        return;
    }

    m_iconified = true;

    /* Get screen number */
    Display *display = QX11Info::display();
    int screen = DefaultScreen(display);
    long dummy;

    XGetWMNormalHints(display, m_window, &m_sizeHint, &dummy);

    /*
     * A simple call to XWithdrawWindow wont do. Here is what we do:
     * 1. Iconify. This will make the application hide all its other windows. For
     *    example, xmms would take off the playlist and equalizer window.
     * 2. Withdraw the window to remove it from the taskbar.
     */
    XIconifyWindow(display, m_window, screen); // good for effects too
    XSync(display, False);
    XWithdrawWindow(display, m_window, screen);

    updateToggleAction();
#endif
}

void TrayItem::closeWindow() {    
    m_parent->closeWindow(m_slotIndex);
}

void TrayItem::doSkipTaskbar() {
#if 0
    set_NET_WM_STATE("_NET_WM_STATE_SKIP_TASKBAR", m_settings.opt[SkipTaskbar]);
#endif
}

void TrayItem::doSkipPager() {
#if 0
    set_NET_WM_STATE("_NET_WM_STATE_SKIP_PAGER", m_settings.opt[SkipPager]);
#endif
}

void TrayItem::doSticky() {
#if 0
    set_NET_WM_STATE("_NET_WM_STATE_STICKY", m_settings.opt[Sticky]);
#endif
}

void TrayItem::setCustomIcon(QString path) {
    m_customIcon = true;
    QPixmap customIcon;
    if (customIcon.load(path)) {
        m_settings.sCustomIcon = path;
    } else {
        customIcon.load(":/images/question.png");
    }

    setIcon(QIcon(customIcon));
}
// BAA Added as test
void TrayItem::hideOptionsMenu() {
    printf("hideOptionsMenu called\n");
}

void TrayItem::selectCustomIcon(bool value) {
    Q_UNUSED(value);

    printf("in selectCustomIcon 1\n");
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
        printf("in selectCustomIcon 2\n");
        setCustomIcon(path);
        m_configFile->setConfigItem(CUSTOM_ICON_KEY, path);
    }
}

void TrayItem::setSkipTaskbar(bool value) {
    m_actionSkipTaskbar->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(SKIP_TASKBAR_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setSkipPager(bool value) {
    m_actionSkipPager->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(SKIP_PAGER_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setSticky(bool value) {
    m_actionSticky->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(STICKY_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setIconifyMinimized(bool value) {
    m_actionIconifyMinimized->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(ICONIFY_IF_MINIMIZED_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setIconifyObscured(bool value) {
    m_actionIconifyObscured->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(ICONIFY_IF_OBSCURED_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setIconifyFocusLost(bool value) {
    m_actionIconifyFocusLost->setChecked(value);
    m_parent->updateConfiguration(m_slotIndex);
    m_configFile->setConfigItem(ICONIFY_IF_FOCUS_LOST_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setLockToDesktop(bool value) {
    printf("setLockToDesktop: %d\n", value);
    m_actionLockToDesktop->setChecked(value);
    m_configFile->setConfigItem(LOCK_TO_DESKTOP_KEY, value);
    m_parent->updateConfiguration(m_slotIndex);
}

void TrayItem::setBalloonTimeout(int value) {
#if 0
    if (value < 0) {
        value = 0;
    }
    m_settings.iBalloonTimeout = value;
    m_actionBalloonTitleChanges->setChecked(value ? true : false);
#endif
}

void TrayItem::setBalloonTimeout(bool value) {
#if 0
    if (!value) {
        setBalloonTimeout(-1);
    } else {
        setBalloonTimeout(nonZeroBalloonTimeout());
    }
#endif
}

void TrayItem::toggleWindow() {
    printf("toggleWindow called\n");
    m_parent->toggleHideShow(m_slotIndex);
}

void TrayItem::trayActivated(QSystemTrayIcon::ActivationReason reason) {
#if 0
    if (reason == QSystemTrayIcon::Trigger) {
        toggleWindow();
    }
#endif
}

bool TrayItem::event(QEvent *e) {
#if 0
    if (e->type() == QEvent::Wheel) {
        QWheelEvent *we = static_cast<QWheelEvent*>(e);
        QPoint delta = we->angleDelta();
        if (!delta.isNull() && delta.y() != 0) {
            if (delta.y() > 0) {
                restoreWindow();
            } else {
                iconifyWindow();
            }
            return true;
        }
    }
#endif
    return QSystemTrayIcon::event(e);
}

void TrayItem::doUndock() {
    printf("in doUndock\n");
//    this->hide();
    m_parent->doUndock(m_slotIndex);
    
}

void TrayItem::undockAll() {
    printf("in undockAll\n");
    m_parent->doUndockAll();
    
}

void TrayItem::minimizeEvent() {
#if 0
    if (m_settings.opt[IconifyMinimized]) {
        iconifyWindow();
    }
#endif
}

void TrayItem::destroyEvent() {
#if 0
    m_window = 0;
    emit dead(this);
#endif
}

#if 0
void TrayItem::propertyChangeEvent(Atom property) {
    if (isBadWindow()) {
        return;
    }

    Display *display = QX11Info::display();
    static Atom WM_NAME         = XInternAtom(display, "WM_NAME", True);
    static Atom WM_ICON         = XInternAtom(display, "WM_ICON", True);
    static Atom WM_STATE        = XInternAtom(display, "WM_STATE", True);
    static Atom _NET_WM_DESKTOP = XInternAtom(display, "_NET_WM_DESKTOP", True);

    if (property == WM_NAME) {
        updateTitle();
    } else if (property == WM_ICON) {
        updateIcon();
    } else if (property == _NET_WM_DESKTOP) {
        XLibUtil::getCardinalProperty(display, m_window, _NET_WM_DESKTOP, &m_desktop);
    } else if (property == WM_STATE) {
        Atom type = None;
        int format;
        unsigned long nitems, after;
        unsigned char *data = 0;
        int r = XGetWindowProperty(display, m_window, WM_STATE, 0, 1, False, AnyPropertyType, &type, &format, &nitems, &after, &data);
        if ((r == Success) && data && (*reinterpret_cast<long *> (data) == IconicState)) {
            // KDE 5.14 started issuing this event when the user changes virtual desktops so
            // a minimizeEvent() should not be executed unless the window is on the currently
            // visible desktop
            if (isOnCurrentDesktop()) {
                minimizeEvent();
            }
        }
        XFree(data);
    }
}
#endif

void TrayItem::obscureEvent() {
#if 0
    if (m_settings.opt[IconifyObscured]) {
        iconifyWindow();
    }
#endif
}

void TrayItem::focusLostEvent() {
#if 0
    // Wait half a second before checking to ensure the window is properly
    // focused when being restored.
    QTime t;
    t.start();
    while (t.elapsed() < 500) {
        qApp->processEvents();
    }

    if (m_settings.opt[IconifyFocusLost] && m_window != XLibUtil::activeWindow(QX11Info::display())) {
        iconifyWindow();
    }
#endif
}

void TrayItem::set_NET_WM_STATE(const char *type, bool set) {    
#if 0
    if (isBadWindow()) {
        return;
    }

    // set, true = add the state to the window. False, remove the state from
    // the window.
    Display *display = QX11Info::display();
    Atom atom = XInternAtom(display, type, False);

    qint64 l[2] = {set ? 1 : 0, static_cast<qint64>(atom)};
    XLibUtil::sendMessage(display, QX11Info::appRootWindow(), m_window, "_NET_WM_STATE", 32, SubstructureNotifyMask, l, sizeof (l));
#endif
}

void TrayItem::readDockedAppName() {
#if 0
    if (isBadWindow()) {
        return;
    }

    Display *display = QX11Info::display();
    XClassHint ch;
    if (XGetClassHint(display, m_window, &ch)) {
        if (ch.res_class) {
            m_dockedAppName = QString(ch.res_class);
        } else if (ch.res_name) {
            m_dockedAppName = QString(ch.res_name);
        }

        if (ch.res_class) {
            XFree(ch.res_class);
        }
        if (ch.res_name) {
            XFree(ch.res_name);
        }
    }
#endif
}

/*
 * Update the title in the tooltip.
 */
void TrayItem::updateTitle() {
    setToolTip(QString("%1 [%2]").arg(m_windowTitle).arg(m_dockedAppName));
    if (m_settings.iBalloonTimeout > 0) {
        // showMessage(m_dockedAppName,/* title,*/ QSystemTrayIcon::Information, m_settings.iBalloonTimeout);
    }
}

#if 0
void TrayItem::updateIcon() {
    if (isBadWindow() || m_customIcon) {
        return;
    }

    setIcon(createIcon(m_window));
}
#endif

void TrayItem::updateToggleAction() {
#if 0
    QString text;
    QIcon icon;

    if (m_iconified) {
        text = tr("Show %1").arg(m_dockedAppName);
        icon = QIcon(":/images/restore.png");
    } else {
        text = tr("Hide %1").arg(m_dockedAppName);
        icon = QIcon(":/images/iconify.png");
    }

    m_actionToggle->setIcon(icon);
    m_actionToggle->setText(text);
#endif
}

void TrayItem::createContextMenu() {
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

    m_actionIconifyObscured = new QAction(tr("Iconify when obscured"), m_optionsMenu);
    m_actionIconifyObscured->setCheckable(true);
    m_configFile->getConfigItem(ICONIFY_IF_OBSCURED_KEY, boolVal);
    m_actionIconifyObscured->setChecked(boolVal);
    connect(m_actionIconifyObscured, SIGNAL(triggered(bool)), this, SLOT(setIconifyObscured(bool)));
    m_optionsMenu->addAction(m_actionIconifyObscured);

    m_actionIconifyFocusLost = new QAction(tr("Iconify when focus lost"), m_optionsMenu);
    m_actionIconifyFocusLost->setCheckable(true);
    m_configFile->getConfigItem(ICONIFY_IF_FOCUS_LOST_KEY, boolVal);
    m_actionIconifyFocusLost->setChecked(boolVal);
    connect(m_actionIconifyFocusLost, SIGNAL(toggled(bool)), this, SLOT(setIconifyFocusLost(bool)));
    m_optionsMenu->addAction(m_actionIconifyFocusLost);

    m_actionLockToDesktop = new QAction(tr("Lock to desktop"), m_optionsMenu);
    m_actionLockToDesktop->setCheckable(true);
    m_configFile->getConfigItem(LOCK_TO_DESKTOP_KEY, boolVal);
    m_actionLockToDesktop->setChecked(boolVal);
    connect(m_actionLockToDesktop, SIGNAL(toggled(bool)), this, SLOT(setLockToDesktop(bool)));
    m_optionsMenu->addAction(m_actionLockToDesktop);

    m_actionBalloonTitleChanges = new QAction(tr("Balloon title changes"), m_optionsMenu);
    m_actionBalloonTitleChanges->setCheckable(true);
    m_configFile->getConfigItem(BALLOON_TIMEOUT_KEY, intVal);
    m_actionBalloonTitleChanges->setChecked(intVal ? true : false);
    connect(m_actionBalloonTitleChanges, SIGNAL(triggered(bool)), this, SLOT(setBalloonTimeout(bool)));
    m_optionsMenu->addAction(m_actionBalloonTitleChanges);

    m_contextMenu->addMenu(m_optionsMenu);

    // Save settings menu
    m_optionsMenu->addSeparator();
    m_defaultsMenu = new QMenu(tr("Save settings"), m_optionsMenu);
    m_defaultsMenu-> setIcon(QIcon(":/images/config.png"));

    m_actionSaveSettingsApp = new QAction(tr("%1 only").arg(m_dockedAppName), m_defaultsMenu);
    connect(m_actionSaveSettingsApp, SIGNAL(triggered()), this, SLOT(saveSettingsApp()));
    m_defaultsMenu->addAction(m_actionSaveSettingsApp);

    m_actionSaveSettingsGlobal = new QAction(tr("Global (all new)"), m_defaultsMenu);
    // connect(m_actionSaveSettingsGlobal, SIGNAL(triggered()), this, SLOT(saveSettingsGlobal()));
    m_defaultsMenu->addAction(m_actionSaveSettingsGlobal);
    connect(m_actionSaveSettingsApp, SIGNAL(triggered()), this, SLOT(saveSettingsApp()));

    m_optionsMenu->addMenu(m_defaultsMenu);
    // ---

//    m_contextMenu->addAction(QIcon(":/images/another.png"), tr("Dock Another"), this, SIGNAL(selectAnother()));
    m_contextMenu->addAction(tr("Undock All"), this, SLOT(undockAll()));
    m_contextMenu->addSeparator();
//    m_actionToggle = new QAction(tr("Toggle"), m_contextMenu);
//    connect(m_actionToggle, SIGNAL(triggered()), this, SLOT(toggleWindow()));
//    m_contextMenu->addAction(m_actionToggle);
    m_contextMenu->addAction(tr("Undock"), this, SLOT(doUndock()));
    m_contextMenu->addAction(QIcon(":/images/close.png"), tr("Close"), this, SLOT(closeWindow()));

    setContextMenu(m_contextMenu);
}

QIcon TrayItem::createIcon(/* Window window */) {
#if 0
    char **window_icon = 0;

    if (!window) {
        return QIcon();
    }

    QPixmap appIcon;
    Display *display = QX11Info::display();
    XWMHints *wm_hints = XGetWMHints(display, window);

    if (wm_hints != 0) {
        if (!(wm_hints->flags & IconMaskHint))
            wm_hints->icon_mask = None;
        /*
         * We act paranoid here. Progams like KSnake has a bug where
         * IconPixmapHint is set but no pixmap (Actually this happens with
         * quite a few KDE 3.x programs) X-(
         */
        if ((wm_hints->flags & IconPixmapHint) && (wm_hints->icon_pixmap)) {
            XpmCreateDataFromPixmap(display, &window_icon, wm_hints->icon_pixmap, wm_hints->icon_mask, 0);
        }
        XFree(wm_hints);
    }

    if (!window_icon) {
        appIcon.load(":/images/question.png");
    } else {
        appIcon = QPixmap(const_cast<const char **> (window_icon));
    }
    if (window_icon) {
        XpmFree(window_icon);
    }
    return QIcon(appIcon);
#endif
#if 1
    return QIcon();
#endif
}

bool TrayItem::isBadWindow() {
#if 0
    Display *display = QX11Info::display();

    if (!XLibUtil::isValidWindowId(display, m_window)) {
        destroyEvent();
        return true;
    }
#endif
    return false;
}

void TrayItem::changeWindowTitle(QString newTitle) {
    m_windowTitle = newTitle;
    updateTitle();
}

// Checks to see if the virtual desktop the window is on is currently 
// displayed. Returns true if it is, otherwise false
bool TrayItem::isOnCurrentDesktop() {
#if 0
    Display *display = QX11Info::display();
    Atom type = None;
    int format;
    unsigned long nitems, after;
    unsigned char *data = 0;

    static Atom _NET_CURRENT_DESKTOP = XInternAtom(display, "_NET_CURRENT_DESKTOP", True);

    long currentDesktop;
    int r = XGetWindowProperty(display, DefaultRootWindow(display), _NET_CURRENT_DESKTOP, 0, 4, False,
                           AnyPropertyType, &type, &format,     
                           &nitems, &after, &data);
    if (r == Success && data) 
        currentDesktop = *reinterpret_cast<long *> (data);
    else
        currentDesktop = m_desktop;

    XFree(data);

    return (currentDesktop == m_desktop);
#else
    return true;
#endif
}

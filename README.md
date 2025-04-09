# WKDocker

Dock most applications to the system tray.

WKDocker provides some of the basic functionality of the 
excellent KDocker (https://github.com/user-none/KDocker) 
but works under Wayland as well as X11.

## Limitations

Because WKDocker uses a KWin Script, it will only work
with KDE.

There is a maximum of 10 windows that can be docked 
at any one time.

## How To Use
WKDocker consists of two parts, the WKDockerApp daemon and
the wkdocker KWin script. The daemon is a user level app
that is set up to be started by the dbus autostart feature.
When the user attempts to dock the first window, the KWin
script makes a dbus call which then starts the daemon

Windows are docked using a KWin HotKey (by default 
Meta+Shift+P).
## Project home

https://github.com/Druco/WKDocker


## Building

### From source

To build the KWin script
```sh
$ cd <top level directory>
$ cd kwin_script
$ make
$ sudo make install
```

To build the daemon
```sh
$ cd <top level directory>
$ cd applet
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

Using KDE system settings, go to
Window Management->KWin Scripts,
and enable the WKDocker script.

Now press Meta+Shift+P and the window that is currently active
should be docked. Left clicking on the new system tray icon
should restore the window to the desktop or iconify it again 
depending on its state.

Right clicking on the system tray icon will bring up a context
menu which allows for the setting of a few configuration items
as well as undocking the window or closing the window. This menu
should look familiar to anyone who has used KDocker just with
fewer options.

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
so it can be started from the command line but should 
typically be started as a user autostart so it is available 
on login.

Once the daemon is running, hitting the 'dockerPickWindow'
HotKey (default is Meta+Shift+P) will cause the active
window to be docked.

## Project home

https://github.com/Druco/WKDocker


## Building

### From source

To build the KWin script
$ cd <top level directory>
$ cd kwin_script
$ make

To build the daemon
$ cd <top level directory>
$ cd applet
$ mkdir build
$ cd build
$ cmake ..
$ make

For test purposes the application can now be run directly
from the build directories. Using KDE system settings, go to
Window Management->KWin Scripts, click on 'Install from File...'
and in the file selection dialog, maneuver to
<top level directory>/kwin_script and select 'Docker.kwinscript'.
It should now be listed under the available scripts and can 
be enabled.

Then in a shell window, 
$ cd <top level directory>/applet/build
$ ./WKDocker

Now press Meta+Shift+P and the window that is currently active
should be docked. Left clicking on the new system tray icon
should restore the window to the desktop or iconify it again 
depending on its state.

Right clicking on the system tray icon will bring up a context
menu which allows for the setting of a few configuration items
as well as undocking the window or closing the window. This menu
should look familiar to anyone who has used KDocker just with
fewer options.

*IMPORTANT*: Close all previous instances of WKDocker that are running before running
a new build. WKDocker is a single instance application.

To do a system wide install, simply go to the 'kwin_script' 
and the 'applet/build' directory and do a 'sudo make install'
in each.

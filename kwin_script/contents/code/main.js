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

const NUM_SLOTS = 10;
const ALREADY_DOCKED = -1;
const SLOTS_FULL = -2;
const NOT_NORMAL_WINDOW = -3;
const TOGGLE_WINDOW_STATE = 4001;
const UNDOCK_WINDOW       = 4002;
const UNDOCK_ALL          = 4003;
const CLOSE_WINDOW        = 4004;
const SETUP_AVAILABLE     = 4005;

var clientList = [{}, {}, {}, {}, {}, {}, {}, {}, {}, {}];
var clientValid = [false, false, false, false, false, false, false, false, false, false];
var currentClientIndex = -1;
var returnval = "default";
var q1 = 0;

function dockerCommandAvailable()
{
    callDBus("org.andtru.wkdocker",
             "/docker",
             "org.andtru.wkdocker.DockerDaemon",
             "requestCommand",
             function(slotIndex, command) {processCommand(slotIndex, command);}
            );
}

function processCommand(slotIndex, command)
{
    currentClientIndex = slotIndex;
    switch (command) {
    case TOGGLE_WINDOW_STATE:
        toggleWindowState(slotIndex, command);
        break;
    case UNDOCK_WINDOW:
        undockWindow(slotIndex);
        break;
    case UNDOCK_ALL:
        undockAll();
        break;
    case CLOSE_WINDOW:
        closeWindow(slotIndex);
        break;
    case SETUP_AVAILABLE:
        getAvailableSetup(slotIndex);
        break
   }
}
    

function toggleWindowState(slotIndex)
{
    var currentDesktop = workspace.currentDesktop;
    var topNormalWindow = 0;

    if (slotIndex < 0 || !clientValid[slotIndex]) {
        return;
    }

    var activeClient = clientList[slotIndex]["WindowID"];

    // If window is on top, on the current desktop, and not minimized then minimize it
    for (var i = workspace.stackingOrder.length - 1; i >= 0; --i) {
        if (workspace.stackingOrder[i].normalWindow) {
            topNormalWindow = workspace.stackingOrder[i];
            break;
        }
    }

    if (activeClient == topNormalWindow &&
        (activeClient.onAllDesktops || activeClient.desktops[0] == currentDesktop) &&
        activeClient.minimized == false) {
        activeClient.minimized = true;
        clientList[slotIndex]["Minimized"] = true;
        activeClient.skipTaskbar = true;
        activeClient.skipSwitcher = true;

    // If window is visible on current desktop but not active, make it active
    } else if ((activeClient.onAllDesktops || activeClient.desktops[0] == currentDesktop) && !activeClient.minimized) {
        workspace.activeWindow = activeClient;
        activeClient.skipTaskbar = clientList[slotIndex]["SkipTaskBar"];
        activeClient.skipSwitcher = clientList[slotIndex]["SkipPager"];
    // Otherwise it is on another desktop, so make it active and either move it
    // to the current desktop (LockToDesktop == false) or switch to the desktop it is on
    } else {
        if (clientList[slotIndex]["LockToDesktop"]){
            if (!activeClient.onAllDesktops && activeClient.moveable) {
                workspace.currentDesktop = activeClient.desktops[0];
            }
        } else if (!activeClient.onAllDesktops){
            activeClient.desktops[0] = currentDesktop;
        }

        activeClient.minimized = false;
        clientList[slotIndex]["Minimized"] = false;
        workspace.activeWindow = activeClient;
        activeClient.skipTaskbar = clientList[slotIndex]["SkipTaskBar"];
        activeClient.skipSwitcher = clientList[slotIndex]["SkipPager"];
    }
}

function undockWindow(slotIndex)
{
    if (clientValid[slotIndex] == true) {
        var activeClient = clientList[slotIndex]["WindowID"];

        activeClient.minimized = false;
        clientList[slotIndex]["Minimized"] = false;
        activeClient.skipTaskbar = false;
        activeClient.skipSwitcher = false;
        clientList[slotIndex] = {};
        clientValid[slotIndex] = false;
    }
}

function getAvailableSetup(slotIndex)
{
    callDBus("org.andtru.wkdocker",
             "/docker",
             "org.andtru.wkdocker.DockerDaemon",
             "requestSetup",
             slotIndex,
             function(a,b,c,d,e,f) {var ws = clientList[a];
                                      if (ws["Initialized"] == false) {
                                          ws["SkipPager"] = b;
                                          ws["SkipTaskBar"] = c;
                                          ws["IcdconifyIfMinimized"] = d;
                                          ws["LockToDesktop"] = e;
                                          ws["Sticky"] = f;
                                          ws["Initialized"] = true;
                                          clientList[a]["WindowID"].minimized = true;
                                      } else {
                                          // Store to local setup vars
                                          ws["SkipPager"] = b;
                                          ws["SkipTaskBar"] = c;
                                          ws["IconifyIfMinimized"] = d;
                                          ws["LockToDesktop"] = e;
                                          ws["Sticky"] = f;

                                          // Activate those that should be done immediately
                                          // but only if not minimized
                                          if (!ws["WindowID"].minimized) {
                                              ws["WindowID"].skipSwitcher = b;
                                              ws["WindowID"].skipTaskbar = c;
                                          }
                                          ws["WindowID"].onAllDesktops = f;
                                      }
                          }
             )
}
             
    
// Second version of pick window. Does not require SetSlot to be called
// first, it just finds the next available slot and uses it. Picks
// active window
function pickWindow()
{
    var selectedWindow = workspace.activeWindow;

    // Check to make sure the window hasn't already been docked
    for (var i = 0; i < NUM_SLOTS; ++i) {
        if (clientValid[i] == true && clientList[i]["WindowID"] == selectedWindow) {
            callDBus("org.andtru.wkdocker", 
                     "/docker", 
                     "org.andtru.wkdocker.DockerDaemon", 
                     "addNewWindow", ALREADY_DOCKED, selectedWindow.resourceClass, "Error");
            return;
        }
    }

    if (!selectedWindow.normalWindow) {
        callDBus("org.andtru.wkdocker", 
                 "/docker", 
                 "org.andtru.wkdocker.DockerDaemon", 
                 "addNewWindow", NOT_NORMAL_WINDOW, selectedWindow.resourceClass, "Error");
        return;
    }
        
    // put the selected window into  the first empty slot 
    for (var i = 0; i < NUM_SLOTS; ++i) {
        if (clientValid[i] == false) {
            currentClientIndex = i;
            clientList[i] = {};
            clientList[i]["WindowID"] = selectedWindow;
            clientList[i]["Initialized"] = false;

            clientValid[i] = true;

            callDBus("org.andtru.wkdocker", 
                     "/docker", 
                     "org.andtru.wkdocker.DockerDaemon", 
                     "addNewWindow", currentClientIndex,
                                     clientList[i]["WindowID"].resourceClass,
                                     clientList[i]["WindowID"].caption);
            selectedWindow.minimizedChanged.connect(onMinimize);

            switch(i) {
            case 0:
                selectedWindow.closed.connect(onClose0);
                selectedWindow.captionChanged.connect(onCaptionChanged0);
                break;
            case 1:
                selectedWindow.closed.connect(onClose1);
                selectedWindow.captionChanged.connect(onCaptionChanged1);
                break;
            case 2:
                selectedWindow.closed.connect(onClose2);
                selectedWindow.captionChanged.connect(onCaptionChanged2);
                break;
            case 3:
                selectedWindow.closed.connect(onClose3);
                selectedWindow.captionChanged.connect(onCaptionChanged3);
                break;
            case 4:
                selectedWindow.closed.connect(onClose4);
                selectedWindow.captionChanged.connect(onCaptionChanged4);
                break;
            case 5:
                selectedWindow.closed.connect(onClose5);
                selectedWindow.captionChanged.connect(onCaptionChanged5);
                break;
            case 6:
                selectedWindow.closed.connect(onClose6);
                selectedWindow.captionChanged.connect(onCaptionChanged6);
                break;
            case 7:
                selectedWindow.closed.connect(onClose7);
                selectedWindow.captionChanged.connect(onCaptionChanged7);
                break;
            case 8:
                selectedWindow.closed.connect(onClose8);
                selectedWindow.captionChanged.connect(onCaptionChanged8);
                break;
            case 9:
                selectedWindow.closed.connect(onClose9);
                selectedWindow.captionChanged.connect(onCaptionChanged9);
                break;
            }
            return;
        }
    }
    // There wasn't an empty slot so notify the companion app
    callDBus("org.andtru.wkdocker", 
             "/docker", 
             "org.andtru.wkdocker.DockerDaemon", 
             "addNewWindow", SLOTS_FULL, selectedWindow.resourceClass);
}

function onMinimize()
{
    //Find if one of our windows has changed
    for (var i=0; i<NUM_SLOTS; ++i) {
        testWindow = clientList[i]["WindowID"];
        if (clientValid[i] && testWindow.minimized != clientList[i]["Minimized"]) {
            clientList[i]["Minimized"] = testWindow.minimized;
            if (testWindow.minimized == true) {
                testWindow.skipTaskbar = true;
                testWindow.skipSwitcher = true;
            } else {
                testWindow.skipTaskbar = clientList[i]["SkipTaskBar"];
                testWindow.skipPager = clientList[i]["SkipPager"];
            }

//            callDBus("org.andtru.menutest", 
//                     "/docker", 
//                     "com.wkdocker.wkdocker.DockerDaemon", 
//                     "onManualMinizeChange", i, testWindow.minimized);
        }
    }
}

function onClose0() {onClose(0);}
function onClose1() {onClose(1);}
function onClose2() {onClose(2);}
function onClose3() {onClose(3);}
function onClose4() {onClose(4);}
function onClose5() {onClose(5);}
function onClose6() {onClose(6);}
function onClose7() {onClose(7);}
function onClose8() {onClose(8);}
function onClose9() {onClose(9);}

function onClose(slotIndex)
{
    clientList[slotIndex] = {};
    clientValid[slotIndex] = false;
    callDBus("org.andtru.wkdocker",
             "/docker",
             "org.andtru.wkdocker.DockerDaemon",
             "onClientClosed",
             currentClientIndex);
}

function onCaptionChanged0() {onCaptionChanged(0);}
function onCaptionChanged1() {onCaptionChanged(1);}
function onCaptionChanged2() {onCaptionChanged(2);}
function onCaptionChanged3() {onCaptionChanged(3);}
function onCaptionChanged4() {onCaptionChanged(4);}
function onCaptionChanged5() {onCaptionChanged(5);}
function onCaptionChanged6() {onCaptionChanged(6);}
function onCaptionChanged7() {onCaptionChanged(7);}
function onCaptionChanged8() {onCaptionChanged(8);}
function onCaptionChanged9() {onCaptionChanged(9);}

function onCaptionChanged(slotIndex)
{
    callDBus("org.andtru.wkdocker",
             "/docker",
             "org.andtru.wkdocker.DockerDaemon",
             "onCaptionChanged",
             slotIndex,
             clientList[slotIndex]["WindowID"].caption
            );
}

function undockAll()
{
    for (var i = 0; i < NUM_SLOTS; ++i) {
        clientList[i] = {};
        clientValid[i] = false;
    }
}

// Close a window
function closeWindow(slotIndex)
{
    clientList[slotIndex]["WindowID"].closeWindow();
}

registerShortcut("pickWindow", "pickWindow", "Meta+Shift+P", pickWindow);
registerShortcut("dockerCommandAvailable", "dockerCommandAvailable", "", dockerCommandAvailable)

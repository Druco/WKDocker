/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2014 Thomas Lübking <thomas.luebking@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

var firefoxClient;
const NUM_SLOTS = 10;
const ALREADY_DOCKED = -1;
const SLOTS_FULL = -2;
const NOT_NORMAL_WINDOW = -3;

var clientList = [{}, {}, {}, {}, {}, {}, {}, {}, {}, {}];
var clientValid = [false, false, false, false, false, false, false, false, false, false];
var currentClientIndex = -1;
var returnval = "default";
var q1 = 0;

function dockerSetSlot1() {
    currentClientIndex = 0;
}
function dockerSetSlot2() {
    currentClientIndex = 1;
}
function dockerSetSlot3() {
    currentClientIndex = 2;
}
function dockerSetSlot4() {
    currentClientIndex = 3;
}

function dockerSetSlot5() {
    currentClientIndex = 4;
}

function dockerSetSlot6() {
    currentClientIndex = 5;
}

function dockerSetSlot7() {
    currentClientIndex = 6;
}

function dockerSetSlot8() {
    currentClientIndex = 7;
}

function dockerSetSlot9() {
    currentClientIndex = 8;
}

function dockerSetSlot10() {
    currentClientIndex = 9;
}

function dockerDockWindow() {
    var currentDesktop = workspace.currentDesktop;
    var topNormalWindow = 0;
    // print("ddw1A");

    if (currentClientIndex < 0 || !clientValid[currentClientIndex]) {
        return;
    }

    var activeClient = clientList[currentClientIndex]["WindowID"];
    // print("ddw2", workspace.stackingOrder.length);
    print("ddw2", activeClient.onAllDesktops, activeClient.desktops.length);
    print("ddw8A", clientList[currentClientIndex]["SkipTaskBar"], clientList[currentClientIndex]["SkipPager"]);
    // If window is on top, on the current desktop, and not minimized then minimize it
    for (var i = workspace.stackingOrder.length - 1; i >= 0; --i) {
        print("ddw8B: ", i, workspace.stackingOrder[i].normalWindow);
        if (workspace.stackingOrder[i].normalWindow) {
            topNormalWindow = workspace.stackingOrder[i];
            break;
        }
    }

    if (activeClient == topNormalWindow &&
        (activeClient.onAllDesktops || activeClient.desktops[0] == currentDesktop) &&
        activeClient.minimized == false) {
        print("ddw3");
        activeClient.minimized = true;
        clientList[currentClientIndex]["Minimized"] = true;
        activeClient.skipTaskbar = true;
        activeClient.skipSwitcher = true;

    // If window is visible on current desktop but not active, make it active
    } else if ((activeClient.onAllDesktops || activeClient.desktops[0] == currentDesktop) && !activeClient.minimized) {
        print("ddw6");
        workspace.activeWindow = activeClient;
        activeClient.skipTaskbar = clientList[currentClientIndex]["SkipTaskBar"];
        activeClient.skipSwitcher = clientList[currentClientIndex]["SkipPager"];
    // Otherwise it is on another desktop, so make it active and either move it
    // to the current desktop (LockToDesktop == false) or switch to the desktop it is on
    } else
    {
        print("ddw4", clientList[currentClientIndex]["LockToDesktop"]);

        if (clientList[currentClientIndex]["LockToDesktop"]){
            if (!activeClient.onAllDesktops && activeClient.moveable) {
                print("ddw5A");
                workspace.currentDesktop = activeClient.desktops[0];
            }
        } else {
            activeClient.desktops[0] = currentDesktop;
        }

        activeClient.minimized = false;
        clientList[currentClientIndex]["Minimized"] = false;
        workspace.activeWindow = activeClient;
        activeClient.skipTaskbar = clientList[currentClientIndex]["SkipTaskBar"];
        activeClient.skipSwitcher = clientList[currentClientIndex]["SkipPager"];
    }
}

function dockerUndockWindow() {
    print("DDW0 *************** ");
    if (clientValid[currentClientIndex] == true) {
        var activeClient = clientList[currentClientIndex]["WindowID"];

        activeClient.minimized = false;
        clientList[currentClientIndex]["Minimized"] = false;
        activeClient.skipTaskbar = false;
        activeClient.skipSwitcher = false;
        clientList[currentClientIndex] = {};
        clientValid[currentClientIndex] = false;
    }
}

function dockerSetupAvailable() {
    print("dSA entry");
    callDBus("org.andtru.menutest",
             "/docker",
             "com.wkdocker.wkdocker.DockerDaemon",
             "requestSetup",
             currentClientIndex,
             function(a,b,c,d,e,f,g,h) {var ws = clientList[a];
                                        print("Setup Callback", ws["Initialized"]);
                                      if (ws["Initialized"] == false) {
                                          ws["SkipPager"] = b;
                                          ws["SkipTaskBar"] = c;
                                          ws["IconifyIfMinimized"] = d;
                                          ws["IconifyIfObscured"] = e;
                                          ws["IconifIfFocusLost"] = f;
                                          ws["LockToDesktop"] = g;
                                          ws["Sticky"] = h;
                                          ws["Initialized"] = true;
                                          print("Initial Callback Pre DDW");
                                          dockerDockWindow();
                                      } else {
                                          // Store to local setup vars
                                          ws["SkipPager"] = b;
                                          ws["SkipTaskBar"] = c;
                                          ws["IconifyIfMinimized"] = d;
                                          ws["IconifyIfObscured"] = e;
                                          ws["IconifIfFocusLost"] = f;
                                          ws["LockToDesktop"] = g;
                                          ws["Sticky"] = h;

                                          // Activate those that should be done immediately
                                          ws["WindowID"].skipSwitcher = b;
                                          ws["WindowID"].skipTaskbar = c;
                                          ws["WindowID"].onAllDesktops = h;
                                      }
                                      print("dSA", a, b, c, d, e, f, g, h);});
}
             
    
// Second version of pick window. Does not require SetSlot to be called
// first, it just finds the next available slot and uses it. Picks
// active window
function pickWindow() {
    var selectedWindow = workspace.activeWindow;
    print("pW entry");

    // Check to make sure the window hasn't already been docked
    for (var i = 0; i < NUM_SLOTS; ++i) {
        print("pW loop1A: ", i, clientValid[i], clientList[i]["WindowID"]);
        if (clientValid[i] == true && clientList[i]["WindowID"] == selectedWindow) {
            callDBus("org.andtru.menutest", 
                     "/docker", 
                     "com.wkdocker.wkdocker.DockerDaemon", 
                     "addNewWindow", ALREADY_DOCKED, selectedWindow.resourceClass);
            return;
        }
    }

    if (!selectedWindow.normalWindow)
    {
        print("Non normal window");
        callDBus("org.andtru.menutest", 
                 "/docker", 
                 "com.wkdocker.wkdocker.DockerDaemon", 
                 "addNewWindow", NOT_NORMAL_WINDOW, selectedWindow.resourceClass);
        return;
    }
        
    print("pW post loop1");
    print("pW post loop1A", selectedWindow.caption);

    // put the selected window into  the first empty slot 
    for (var i = 0; i < NUM_SLOTS; ++i) {
        print("pw loop2A: ", i, clientValid[i]);
        if (clientValid[i] == false) {
            print("pW loop2");
            currentClientIndex = i;
            clientList[i] = {};
            clientList[i]["WindowID"] = selectedWindow;
            clientList[i]["Initialized"] = false;
            
            // If you want ot use the top window rather than the active window
            // clientList[i] = workspace.stackingOrder[workspace.stackingOrder.length - 2];
            clientValid[i] = true;

            callDBus("org.andtru.menutest", 
                     "/docker", 
                     "com.wkdocker.wkdocker.DockerDaemon", 
                     "addNewWindow", currentClientIndex,
                                     clientList[i]["WindowID"].resourceClass,
                                     clientList[i]["WindowID"].caption);
            selectedWindow.minimizedChanged.connect(onMinimize);
            print("switch", i);
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
    print("pW error Exit");

    callDBus("org.andtru.menutest", 
             "/docker", 
             "com.wkdocker.wkdocker.DockerDaemon", 
             "addNewWindow", SLOTS_FULL, selectedWindow.resourceClass);
}

function onMinimize() {
    print("in onMinimize");

    //Find if one of our windows has changed
    for (var i=0; i<NUM_SLOTS; ++i) {
        testWindow = clientList[i]["WindowID"];
        if (clientValid[i] && testWindow.minimized != clientList[i]["Minimized"]) {
            print(testWindow,  workspace.activeWindow);
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

function onClose(slotIndex) {
    clientList[slotIndex] = {};
    clientValid[slotIndex] = false;
    print("onClose", slotIndex);
    callDBus("org.andtru.menutest",
             "/docker",
             "com.wkdocker.wkdocker.DockerDaemon",
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

function onCaptionChanged(slotIndex) {
    print("onCaptionChanged", slotIndex, clientList[slotIndex].caption);
    callDBus("org.andtru.menutest",
             "/docker",
             "com.wkdocker.wkdocker.DockerDaemon",
             "onCaptionChanged",
             currentClientIndex,
             clientList[slotIndex]["WindowID"].caption
            );
}

// Temporary only test version
function undockAll() {
    for (var i = 0; i < NUM_SLOTS; ++i) {
        clientList[i] = {};
        clientValid[i] = false;
    }
}

// Close a window
function dockerCloseWindow() {
    currentWindow = clientList[currentClientIndex]["WindowID"];
    currentWindow.closeWindow();
}

print("Inside minimizeFirefox", "Again");
registerShortcut("dockerSetSlot1", "dockerSetSlot1", "", dockerSetSlot1);
registerShortcut("dockerSetSlot2", "dockerSetSlot2", "", dockerSetSlot2);
registerShortcut("dockerSetSlot3", "dockerSetSlot3", "", dockerSetSlot3);
registerShortcut("dockerSetSlot4", "dockerSetSlot4", "", dockerSetSlot4);
registerShortcut("dockerSetSlot5", "dockerSetSlot5", "", dockerSetSlot5);
registerShortcut("dockerSetSlot6", "dockerSetSlot6", "", dockerSetSlot6);
registerShortcut("dockerSetSlot7", "dockerSetSlot7", "", dockerSetSlot7);
registerShortcut("dockerSetSlot8", "dockerSetSlot8", "", dockerSetSlot8);
registerShortcut("dockerSetSlot9", "dockerSetSlot9", "", dockerSetSlot9);
registerShortcut("dockerSetSlot10", "dockerSetSlot10", "", dockerSetSlot10);
registerShortcut("dockerDockWindow", "dockerDockWindow", "", dockerDockWindow);
registerShortcut("dockerUndockWindow", "dockerUndockWindow", "", dockerUndockWindow);
registerShortcut("pickWindow", "pickWindow", "Meta+Shift+P", pickWindow);
registerShortcut("undockAll", "undockAll", "Meta+Shift+U", undockAll);
registerShortcut("dockerSetupAvailable", "dockerSetupAvailable", "", dockerSetupAvailable);
registerShortcut("dockerCloseWindow", "dockerCloseWindow", "", dockerCloseWindow);

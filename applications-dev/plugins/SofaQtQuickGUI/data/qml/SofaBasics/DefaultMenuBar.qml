/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import "qrc:/SofaCommon/SofaSettingsScript.js" as SofaSettingsScript
import SofaApplication 1.0

MenuBar {
    id: menuBar

    property var sofaScene: null

    property list<QtObject> objects: [

        // action
        Action {
            id: openAction
            text: "&Open..."
            shortcut: "Ctrl+O"
            onTriggered: openSofaSceneDialog.open();
            tooltip: "Open a SofaScene"
        },

        Action {
            id: openRecentAction
            onTriggered: {
                var title = source.text.toString();
                var source = title.replace(/^.*"(.*)"$/m, "$1");
                sofaScenePath = source;
                sofaScene.source = "file:" + source
            }
        },

        Action {
            id: clearRecentAction
            text: "&Clear"
            onTriggered: SofaSettingsScript.Recent.clear();
            tooltip: "Clear history"
        },

        Action {
            id: reloadAction
            text: "&Reload"
            shortcut: "Ctrl+R"
            onTriggered: sofaScene.reload();
            tooltip: "Reload the Sofa Scene"
        },

//        Action {
//            id: saveAction
//            text: "&Save"
//            shortcut: "Ctrl+S"
//            onTriggered: if(0 == filePath.length) saveSofaSceneDialog.open(); else sofaScene.save(filePath);
//            tooltip: "Save the Sofa Scene"
//        },

        Action {
            id: saveScreenshotAction
            text: "&Save Screenshot"
            shortcut: "s"
            onTriggered: sofaScene.takeScreenshot();
            tooltip: "Save a screenshot of the current Sofa Scene"
        },

        Action {
            id: saveScreenshotAsAction
            text: "&Save Screenshot As..."
            tooltip: "Save a screenshot of the current Sofa Scene at a specific location"
            onTriggered: saveSofaSceneDialog.open();
        },

        Action
        {
            id: exitAction
            text: "&Exit"
            shortcut: "Ctrl+Q"
            onTriggered: close()
        },

        Action {
            id: simulateAction
            text: "&Simulate"
            shortcut: "Space"
            tooltip: "Open a Sofa Scene"
            checkable: true
            checked: false
            onTriggered: if(sofaScene) sofaScene.play = checked

            property var sofaSceneConnection: Connections {
                target: sofaScene
                onPlayChanged: simulateAction.checked = sofaScene.play
            }
        },

        MessageDialog {
            id: aboutDialog
            title: "About"
            text: "Welcome in the " + Qt.application.name +" Application"
            onAccepted: visible = false
        },

        Action
        {
            id: aboutAction
            text: "&About"
            tooltip: "What is this application ?"
            onTriggered: aboutDialog.visible = true;
        }
    ]

    Menu {
        title: "&File"
        visible: true

        MenuItem {action: openAction}
        Menu {
            id: recentMenu
            title: "Recent sofa scenes"

            visible: 0 !== items.length

            function update() {
                recentMenu.clear();
                var sofaSceneList = SofaSettingsScript.Recent.sofaSceneList();
                if(0 === sofaSceneList.length)
                    return;

                for(var j = 0; j < sofaSceneList.length; ++j) {
                    var sofaSceneSource = sofaSceneList[j];
                    if(0 === sofaSceneSource.length)
                        continue;

                    var sofaSceneName = sofaSceneSource.replace(/^.*[//\\]/m, "");
                    var title = j.toString() + " - " + sofaSceneName + " - \"" + sofaSceneSource + "\"";

                    var openRecentItem = recentMenu.addItem(title);
                    openRecentItem.action = openRecentAction;

                    if(10 === recentMenu.items.length)
                        break;
                }

                if(0 === recentMenu.items.length)
                    return;

                recentMenu.addSeparator();
                var clearRecentItem = recentMenu.addItem("Clear");
                clearRecentItem.action = clearRecentAction;
            }

            Component.onCompleted: recentMenu.update()

            Connections {
                target: SofaSettingsScript.Recent
                onSofaScenesChanged: recentMenu.update()
            }
        }

        MenuItem {action: reloadAction}
        //MenuItem {action: saveAction}
        //MenuItem {action: saveAsAction}
        MenuItem {action:saveScreenshotAction }
        MenuSeparator {}
        MenuItem {action: exitAction}
    }

    Menu {
        title: "&Simulation"
        visible: true
        enabled: sofaScene && sofaScene.ready

        MenuItem {action: simulateAction}
    }

/*
    Menu {
        title: "&Edit"
        //MenuItem {action: cutAction}
        //MenuItem {action: copyAction}
        //MenuItem {action: pasteAction}
        //MenuSeparator {}
        MenuItem {
            text: "Empty"
            enabled: false
        }
    }
    Menu {
        title: "&View"
        MenuItem {
            text: "Empty"
            enabled: false
        }
    }
*/
    Menu {
        title: "&Help"
        MenuItem {action: aboutAction}
    }
}

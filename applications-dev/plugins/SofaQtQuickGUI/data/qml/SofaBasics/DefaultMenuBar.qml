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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaApplication 1.0

MenuBar {
    id: menuBar

    height: 25

    hoverEnabled: true
    function replaceText(txt) {
        var index = txt.indexOf("&");
        if(index >= 0)
            txt = txt.replace(txt.substr(index, 2), ("<u>" + txt.substr(index + 1, 1) +"</u>"));
        return txt;
    }
    delegate: MenuBarItem {
        id: menuBarItem
        contentItem: Text {
            text: replaceText(menuBarItem.text)
            font: menuBarItem.font
            opacity: enabled ? 1.0 : 0.3
            color: "black"
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
        }

        background: Rectangle {
            y: 2
            height: 20
            radius: 4
            opacity: enabled ? 1.0 : 0.2
            color: menuBarItem.highlighted ? "lightblue" : "transparent"
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightblue" }
                GradientStop { position: 0.9; color: "lightblue" }
                GradientStop { position: 1.0; color: "white" }
            }
        }
    }

    background: Rectangle {
        implicitWidth: 25
        implicitHeight: 25
        color: "grey"
    }
    property var sofaScene: null

    property list<QtObject> objects: [
        Action {
            id: openAction
            text: "&Open..."
            shortcut: "Ctrl+O"
            onTriggered: openSofaSceneDialog.open();
//            tooltip: "Open a SofaScene"
        },

        Action {
            id: openRecentAction
            onTriggered: {
                var title = source.text.toString();
                var sceneSource = title.replace(/^.*"(.*)"$/m, "$1");
                sofaScene.source = "file:" + sceneSource;
            }
        },

        Action {
            id: clearRecentAction
            text: "&Clear"
            onTriggered: SofaApplication.sceneSettings.clearRecents();
//            tooltip: "Clear history"
        },

        Action {
            id: reloadAction
            text: "&Reload"
            shortcut: "Ctrl+R"
            onTriggered: sofaScene.reload();
//            tooltip: "Reload the Sofa Scene"
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
//            tooltip: "Open a Sofa Scene"
            checkable: true
            checked: false
            onTriggered: if(sofaScene) sofaScene.animate = checked

            property var sofaSceneConnection: Connections {
                target: sofaScene
                onAnimateChanged: simulateAction.checked = sofaScene.animate
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
//            tooltip: "What is this application ?"
            onTriggered: aboutDialog.visible = true;
        }
    ]

    Menu {
        title: "&File"
        visible: true

        MenuItem {action: openAction}
        Menu {
            id: recentMenu
            title: "Open recent"
            property var items: []
            visible: 0 !== items.length

//            function update() {
//                recentMenu.items.clear();
//                var sofaSceneRecentsList = SofaApplication.sceneSettings.recentsList();
//                if(0 === sofaSceneRecentsList.length)
//                    return;

//                for(var j = 0; j < sofaSceneRecentsList.length; ++j) {
//                    var sofaSceneSource = sofaSceneRecentsList[j];
//                    if(0 === sofaSceneSource.length)
//                        continue;

//                    var sofaSceneName = sofaSceneSource.replace(/^.*[//\\]/m, "");
//                    var title = j.toString() + " - " + sofaSceneName + " - \"" + sofaSceneSource + "\"";

//                    var openRecentItem = recentMenu.addItem(title);
//                    openRecentItem.action = openRecentAction;

//                    if(50 === recentMenu.items.length)
//                        break;
//                }

//                if(0 === recentMenu.items.length)
//                    return;

//                recentMenu.addSeparator();
//                var clearRecentItem = recentMenu.addItem("Clear");
//                clearRecentItem.action = clearRecentAction;
//            }

//            Component.onCompleted: recentMenu.update()

            Connections {
                target: SofaApplication.sceneSettings
                onSofaSceneRecentsChanged: recentMenu.update()
            }
        }

        MenuItem {action: reloadAction}
        MenuItem { text: "Save (TODO)"; enabled : false }
        MenuItem { text: "Save as...(TODO)"; enabled : false }
        MenuItem { text: "Export as...(TODO)"; enabled : false }
        MenuSeparator {}
        MenuItem {action: exitAction}
    }

    Menu {
        title: "&Edit"
        MenuItem { text : "Cut (TODO)"; enabled : false }
        MenuItem { text : "Copy (TODO)"; enabled : false }
        MenuItem { text : "Paste (TODO)"; enabled : false }
    }

    Menu {
        title: "&Scene"
        MenuItem { text: "Reload (TODO)"; enabled : false}
        MenuSeparator {}
        MenuItem { text: "Play (TODO)"; enabled : false}
        MenuItem { text: "Pause (TODO)"; enabled : false}
        MenuItem { text: "Stop (TODO)"; enabled : false }
        MenuSeparator {}
        MenuItem { text: "Add node (TODO)"; enabled : false}
        MenuItem { text: "Add object (TODO)"; enabled : false}
    }

    Menu {
        title: "&Windows"
        MenuItem { text: "Plugins store (TODO)"; enabled : false}
    }

    Menu {
        title: "&Help"
        MenuItem { text: "Help (TODO)"; enabled : false }
        MenuItem { text: "Tutorials (TODO)"; enabled : false }
        MenuItem {action: aboutAction}
    }
}

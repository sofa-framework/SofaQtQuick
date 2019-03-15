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

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaApplication 1.0
import SofaBasics 1.0

MenuBar {
    id: menuBar

    property var sofaScene: null

    property list<QtObject> objects: [
        Action {
            id: openAction
            text: "&Open..."
            shortcut: "Ctrl+O"
            onTriggered: openSofaSceneDialog.open();
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
        },

        Action {
            id: reloadAction
            text: "&Reload"
            shortcut: "Ctrl+R"
            onTriggered: sofaScene.reload();
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
            onTriggered: aboutDialog.visible = true;
        }
    ]

    SofaMenu {
        title: "&File"
        SofaMenuItem {
            action: openAction
            ToolTip {
                text: "Open a Scene file"
                description: "accepted formats: .scn | .pyscn | .py"
            }
        }
        SofaMenu {
            id: recentMenu
            title: "Open recent"
            property var items: []
            visible: 0 !== items.length
            function update() {
                recentMenu.items = []
                var sofaSceneRecentsList = SofaApplication.sceneSettings.recentsList();
                if ( 0 === sofaSceneRecentsList.length)
                {
                    return;
                }

                for(var j = 0; j < sofaSceneRecentsList.length; ++j) {
                    var sofaSceneSource = sofaSceneRecentsList[j];
                    if(0 === sofaSceneSource.length) {
                        continue;
                    }

                    var sofaSceneName = sofaSceneSource.replace(/^.*[//\\]/m, "");
                    var title = j.toString() + " - " + sofaSceneName + " - \"" + sofaSceneSource + "\"";
                    console.log(title)

                    var menuEntry = menuItem.createObject(menuItem, { text: title, action: openRecentAction })
                    var openRecentItem = recentMenu.addItem(menuEntry)

                    if (50 === recentMenu.items.length)
                        break;
                }
                if(0 === recentMenu.items.length)
                    return;

                recentMenu.addSeparator();
                var clearRecentItem = recentMenu.addItem(menuItem.createObject(menuItem, { text: "Clear Recent", action: clearRecentAction }))
            }
            Component {
                id: menuItem
                SofaMenuItem {

                }
            }
            Component.onCompleted: recentMenu.update()

            Connections {
                target: SofaApplication.sceneSettings
                onSofaSceneRecentsChanged: recentMenu.update()
            }
        }

        SofaMenuItem {
            action: reloadAction
            ToolTip {
                text: "Reload the Sofa Scene"
            }
        }
        SofaMenuItem { enabled: false; text: "Save (TODO)" }
        SofaMenuItem { text: "Save as...(TODO)"; enabled : false }
        SofaMenuItem { text: "Export as...(TODO)"; enabled : false }
        MenuSeparator {}
        SofaMenuItem {action: exitAction}
    }


    SofaMenu {
        title: "&Edit"
        SofaMenuItem { text: "Cut (TODO)"; enabled: false }
        SofaMenuItem { text : "Copy (TODO)"; enabled : false }
        SofaMenuItem { text : "Paste (TODO)"; enabled : false }
    }

    SofaMenu {
        title: "&Scene"
        SofaMenuItem { text: "Reload (TODO)"; enabled : false}
        MenuSeparator {}
        SofaMenuItem { text: "Play (TODO)"; enabled : true; checkable: true; checked: true}
        SofaMenuItem { text: "Pause (TODO)"; enabled : false; checkable: true; checked: false}
        SofaMenuItem { text: "Stop (TODO)"; enabled : false }
        MenuSeparator {}
        SofaMenuItem { text: "Add node (TODO)"; enabled : false}
        SofaMenuItem { text: "Add object (TODO)"; enabled : false}
    }

    SofaMenu {
        title: "&Windows"
        SofaMenuItem { text: "Plugins store (TODO)"; enabled : true}
    }

    SofaMenu {
        title: "&Help"
        SofaMenuItem { text: "Help (TODO)"; enabled : false }
        SofaMenuItem { text: "Tutorials (TODO)"; enabled : false }
        SofaMenuItem {
            action: aboutAction
            ToolTip {
                text: "About runSofa2"
            }
        }
    }
}

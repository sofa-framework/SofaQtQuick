
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import SofaBasics 1.0
import SofaViewListModel 1.0


MenuBar {
    id: menuBar
    property var sofaApplication: null

    Item {
        id: internal_params

        property string recentsString: sofaApplication.sceneSettings.sofaSceneRecents
        onRecentsStringChanged: { recentScenesList = recentsString.split(';') }

        property string sceneUrl: openSofaSceneDialog.sofaSceneFileUrl
        onSceneUrlChanged: {
            if (sofaApplication.sofaScene)
                sofaApplication.sofaScene.source = sceneUrl
        }
        property var recentScenesList: []
    }



    Menu {
        id: fileMenuID
        title: qsTr("&File")
        MenuItem {
            id: openMenuItem
            text: "&Open..."

            function openDialog() {
                openSofaSceneDialog.open()
            }

            Shortcut {
                sequence: StandardKey.Open
                context: Qt.ApplicationShortcut
                onActivated: openMenuItem.openDialog()
            }
            onTriggered: {
                openMenuItem.openDialog()
            }

            SofaSceneFileDialog { id: openSofaSceneDialog }

        }


        Menu {
            id: recentMenu
            title: "Open recent"
            enabled: sofaApplication.sceneSettings.sofaSceneRecents !== ""

            implicitHeight: contentHeight
            property var recentScenesModel: ListModel { id: scenesModel }
            property var modelList: internal_params.recentScenesList
            onModelListChanged: {
                scenesModel.clear()
                for (var i = 0 ; i < modelList.length ; i++)
                {
                    if (modelList[i] === "")
                        continue;
                    scenesModel.append({"index": i, "title": modelList[i].replace(/^.*[//\\]/m, ""), "fileUrl": modelList[i]})
                }
            }

            ColumnLayout {
                Repeater {
                    model: scenesModel
                    MenuItem {
                        text: model.index + " - " + model.title
                        onTriggered: {
                            internal_params.sceneUrl = model.fileUrl
                        }
                    }
                }
                MenuSeparator { visible: sofaApplication.sceneSettings.sofaSceneRecents !== "" }
                MenuItem {
                    enabled: sofaApplication.sceneSettings.sofaSceneRecents !== ""
                    text: "Clear Recents"
                    onTriggered: {
                        sofaApplication.sceneSettings.sofaSceneRecents = ""
                    }
                }
            }
        }
        MenuItem {
            id: reloadMenuItem
            text: "&Reload"

            function reload() {
                sofaApplication.sofaScene.reload()
            }
            Shortcut {
                sequence: StandardKey.Refresh
                context: Qt.ApplicationShortcut
                onActivated: reloadMenuItem.reload()
            }
            onTriggered: {
                reloadMenuItem.reload()
            }
        }
        MenuItem { text: "Save (TODO)"; enabled: false }
        MenuItem { text: "Save as...(TODO)"; enabled : false }
        MenuItem { text: "Export as...(TODO)"; enabled : false }
        MenuSeparator {}

        MenuItem {
            id: exitMenuItem
            text: "&Exit"

            function exit() {
                close()
            }
            Shortcut {
                sequence: StandardKey.Quit
                context: Qt.ApplicationShortcut
                onActivated: exitMenuItem.exit()
            }
            onTriggered: {
                exitMenuItem.exit()
            }
        }
    }


    Menu {
        title: "&Edit"
        MenuItem { text: "Cut (TODO)"; enabled: false }
        MenuItem { text : "Copy (TODO)"; enabled : false }
        MenuItem { text : "Paste (TODO)"; enabled : false }
    }

    Menu {
        title: "&Scene"
        MenuItem { text: "Animate (TODO)"; enabled : false; checkable: true; checked: true
            Shortcut {
                sequence: "Ctrl+Space"
                context: Qt.ApplicationShortcut
                onActivated: {}
            }
            ToolTip {
                text: qsTr("Play / Pause the simulation")
            }

        }
        MenuSeparator {}
        MenuItem { text: "Add node (TODO)"; enabled : false }
        MenuItem { text: "Add object (TODO)"; enabled : false }
    }



    Menu {
        title: "&Windows"

        ColumnLayout {
            MenuItem { text: "Plugins store (TODO)"; enabled: false
                ToolTip {
                    text: qsTr("Opens SOFA's plugin store")
                    description: "The plugin store allows you to install additional plugins for your project"
                }
            }
            MenuSeparator {}

            Repeater {
                model: sofaApplication.sofaViewListModel
                MenuItem {
                    text: model.name
                    onTriggered: {
                        windowComponent.createObject(sofaApplication, {
                                                         "source": "file:///"+model.filePath,
                                                         "title" : model.name
                                                     });
                    }
                    ToolTip {
                        text: qsTr("Open " + model.name + " in a new Window")
                    }

                }
            }
            MenuSeparator {}
            MenuItem { text: "Add QML Views (TODO)"; enabled: false
                ToolTip {
                    text: "Adds additional views"
                    description: "Click here to select additional directories containing qml views"
                }
            }
        }
        Component {
            id: windowComponent

            Window {
                property url source

                id: window
                width: 400
                height: 600
                modality: Qt.NonModal
                flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
                visible: true
                color: sofaApplication.style.contentBackgroundColor

                Loader {
                    id: loader
                    anchors.fill: parent
                    source: window.source
                }
            }
        }

    }

    Menu {
        title: "&Help"
        MessageDialog {
            id: aboutDialog
            title: "About"
            text: "Welcome in " + Qt.application.name +"!"
            onAccepted: visible = false
            visible: false
        }

        MenuItem { text: "Help (TODO)"; enabled : false }
        MenuItem { text: "Tutorials (TODO)"; enabled : false }
        MenuItem {
            text: "&About"
            onTriggered: aboutDialog.visible = true;

            ToolTip {
                text: "About runSofa2"
            }
        }
    }
}


import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import SofaBasics 1.0
import SofaViewListModel 1.0
import SofaApplication 1.0
import SofaWidgets 1.0

MenuBar {
    id: menuBar

    Menu {
        id: fileMenuID
        title: qsTr("&File")

        MenuItem {
            id: newProject
            text: qsTr("New Project")
            MessageDialog {
                id: newProjectErrorDialog
                standardButtons: StandardButton.Ok
                icon: StandardIcon.Critical
                informativeText: qsTr("Error:")
                detailedText: qsTr("New project folders must be empty")
            }

            function openDialog() {
                SofaApplication.currentProject.newProject()
                SofaApplication.sceneSettings.addRecent(SofaApplication.sofaScene.source)
                SofaApplication.projectSettings.addRecent(SofaApplication.currentProject.rootDirPath)
            }
            Shortcut {
                sequence: StandardKey.New
                context: Qt.ApplicationShortcut
                onActivated: newProject.openDialog()
            }
            onTriggered: {
                newProject.openDialog()
            }
        }
        MenuItem {
            id: openProject
            text: qsTr("Open Project...")

            function openDialog() {
                SofaApplication.currentProject.openProject()
                SofaApplication.sceneSettings.addRecent(SofaApplication.sofaScene.source)
                SofaApplication.projectSettings.addRecent(SofaApplication.currentProject.rootDirPath)
            }
            Shortcut {
                sequence: StandardKey.Open
                context: Qt.ApplicationShortcut
                onActivated: openProject.openDialog()
            }
            onTriggered: {
                openProject.openDialog()
            }
        }

        MenuItem {
            id: importProject
            text: qsTr("Import Project...")

            onTriggered: {
                SofaApplication.currentProject.importProject();
                SofaApplication.sceneSettings.addRecent(SofaApplication.sofaScene.source)

            }
        }

        MenuItem {
            id: exportProject
            text: qsTr("Export Current Project")
            onTriggered: {
                SofaApplication.currentProject.exportProject()
            }
        }

        MenuItem {
            id: saveAsNewProject
            text: "&Save Scene as New Project..."
            enabled: false
            onTriggered: {
            }
        }

        MenuSeparator {}

        MenuItem {
            id: openMenuItem
            text: "&Open..."

            Shortcut {
                sequence: StandardKey.Open
                context: Qt.ApplicationShortcut
                onActivated: SofaApplication.sofaScene.openScene(SofaApplication.currentProject.rootDir);

            }
            onTriggered: {
                SofaApplication.sofaScene.openScene(SofaApplication.currentProject.rootDir);
            }

        }


        Menu {
            id: recentMenu
            title: "Open recent"
            enabled: SofaApplication.sceneSettings.sofaSceneRecents !== ""

            implicitHeight: contentHeight
            ListModel { id: scenesModel }
            property var modelList: SofaApplication.sceneSettings.sofaSceneRecents
            onModelListChanged: {
                scenesModel.clear()
                var mlist = modelList.split(";")
                for (var i = 0 ; i < mlist.length ; i++)
                {
                    if (mlist[i] === "")
                        continue;
                    scenesModel.append({"index": i, "title": mlist[i].replace(/^.*[//\\]/m, ""), "fileUrl": mlist[i]})
                }
            }

            ColumnLayout {
                Repeater {
                    model: scenesModel
                    MenuItem {
                        text: model.index + " - " + model.title
                        onTriggered: {
                            fileMenuID.close()
                           SofaApplication.sofaScene.source = model.fileUrl
                        }
                    }
                }
                MenuSeparator { visible: SofaApplication.sceneSettings.sofaSceneRecents !== "" }
                MenuItem {
                    enabled: SofaApplication.sceneSettings.sofaSceneRecents !== ""
                    text: "Clear Recents"
                    onTriggered: {
                        fileMenuID.close()
                        SofaApplication.sceneSettings.sofaSceneRecents = ""
                    }
                }
            }
        }
        MenuItem {
            id: reloadMenuItem
            text: "&Reload"

            Shortcut {
                sequence: StandardKey.Refresh
                context: Qt.ApplicationShortcut
                onActivated: SofaApplication.sofaScene.reloadScene()
            }
            onTriggered: {
                fileMenuID.close()
                SofaApplication.sofaScene.reloadScene()
            }
        }

        MenuItem {
            text: "Save";
            id: saveItem

            MessageDialog {
                id: saveDialog
                title: "Overwrite?"
                icon: StandardIcon.Question
                text: SofaApplication.sofaScene.path + " already exists. Replace?"
                detailedText: "The previous version of the file will be backed up in a separate file suffixed '.backup'"
                standardButtons: StandardButton.Yes | StandardButton.Abort
                onYes: SofaApplication.sofaScene.saveScene()
            }

            Shortcut {
                sequence: StandardKey.Save
                context: Qt.ApplicationShortcut
                onActivated: saveDialog.open()
            }
            onTriggered:  { saveDialog.open() }
        }
        MenuItem {
            id: saveAs
            text: "Save as..."



            Shortcut {
                sequence: StandardKey.SaveAs
                context: Qt.ApplicationShortcut
                onActivated: SofaApplication.sofaScene.saveSceneAs(SofaApplication.currentProject.rootDir)
            }
            onTriggered: SofaApplication.sofaScene.saveSceneAs(SofaApplication.currentProject.rootDir)
        }
        MenuItem {
            text: "Export as...(TODO)"
            enabled : false
            onTriggered: SofaApplication.sofaScene.exportSceneAs(SofaApplication.currentProject.rootDir)
        }


        MenuItem {
            id: newScene
            text: "&New Scene..."
            enabled: true
            onTriggered: {
                SofaApplication.sofaScene.newScene()
            }
        }
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
        id: windowMenu
        title: "&Windows"

        ColumnLayout {


            MenuItem {
                text: "Open store..."
                onTriggered: {
                    var o = windowGraph.createObject(root, {
                                                             "source": "qrc:///SofaViews/WebBrowserView.qml",
                                                             "title" : "Sofa Ressources Repository",
                                                             "url": "https://github.com/SofaDefrost/SPM/wiki/Sofa-Ressources",
                                                             "width" : 800,
                                                             "height": 600,
                                                         });
//                    console.log(o.item)
                    windowMenu.close()
                }
                ToolTip {
                    text: qsTr("Opens SOFA's store")
                    description: "The plugin store allows you to install additional plugins for your project"
                }
            }

            MenuSeparator {}

            Repeater {
                model: SofaApplication.sofaViewListModel
                MenuItem {
                    text: model.name
                    onTriggered: {
                        windowComponent.createObject(menuBar,
                                                     {
                                                         "source": "file:///"+model.filePath,
                                                         "title" : model.name
                                                     });
                        windowMenu.close()
                    }
                    ToolTip {
                        text: qsTr("Open " + model.name + " in a new Window")
                    }

                }
            }
            MenuSeparator {}

            MenuItem {
                text: "Connections";
                onTriggered: {
                    GraphView.open()
                    windowMenu.close()
                }
                ToolTip {
                    text: qsTr("Open the default GraphView")
                    description: "To visualize the connection between different components"
                }
            }

            MenuItem {
                text: "Profiler";
                onTriggered: {
                    profilerWidget.visible = true

                    windowMenu.close()
                }
                ToolTip {
                    text: qsTr("Open the default Profiler")
                    description: "To visualize where the computation take place"
                }

                Profiler {
                    id: profilerWidget
                    visible: false
                }
            }

            MenuItem { text: "Add QML Views (TODO)"; enabled: false
                ToolTip {
                    text: "Adds additional views"
                    description: "Click here to select additional directories containing qml views"
                }
            }
        }

        Component {
            id: windowGraph

            Window {
                property url source
                property url url

                id: window
                width: 600
                height: 400
                modality: Qt.NonModal
                flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
                visible: true
                color: SofaApplication.style.contentBackgroundColor

                Loader {
                    id: loader
                    anchors.fill: parent
                    source: window.source
                    onLoaded: { item.url = url }
                }
            }
        }

        Component {
            id: windowComponent

            Window {
                property url source

                id: window
                width: 600
                height: 400
                modality: Qt.NonModal
                flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
                visible: true
                color: SofaApplication.style.contentBackgroundColor

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

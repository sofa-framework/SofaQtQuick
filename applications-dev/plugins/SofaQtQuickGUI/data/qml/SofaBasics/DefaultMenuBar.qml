
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import SofaBasics 1.0
import SofaViewListModel 1.0
import GraphView 1.0
import ProfilerView 1.0

MenuBar {
    id: menuBar
    property var sofaApplication: null

    Item {
        id: internal_params

        property string sceneUrl: openSofaSceneDialog.sofaSceneFileUrl
        onSceneUrlChanged: {
            if (sofaApplication.sofaScene)
                sofaApplication.sofaScene.source = sceneUrl
        }
    }

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

            FileDialog {
                id: newProjectDialog
                selectFolder: true
                selectExisting: false
                onAccepted: {
                    if (Qt.resolvedUrl(fileUrl)) {
                        var scene = sofaApplication.currentProject.createProject(fileUrl)
                        sofaApplication.projectSettings.addRecent(fileUrl)
                        sofaApplication.sofaScene.source = scene
                    }
                    else
                        newProjectErrorDialog.open()
                }
            }

            function openDialog() {
                newProjectDialog.open()
            }
            Shortcut {
                sequence: StandardKey.Open
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
            FileDialog {
                id: openProjectDialog
                title: "Please choose a project directory"
                folder: shortcuts.home
                selectFolder: true
                sidebarVisible: true
                onAccepted: {
                    sofaApplication.projectSettings.addRecent(fileUrl)
                }
            }

            function openDialog() {
                openProjectDialog.open()
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
            FileDialog {
                id: importProjectDialog
                title: "Please choose a project directory"
                folder: shortcuts.home
                selectFolder: false
                selectExisting: false
                sidebarVisible: true
                nameFilters: ["Archives (*.zip)", "All files (*)"]
                onAccepted: {
                    var extractedFolder = sofaApplication.currentProject.importProject(fileUrl);
                    console.error(extractedFolder)
                    sofaApplication.projectSettings.addRecent("file://" + extractedFolder)
                }
            }

            onTriggered: {
                importProjectDialog.open()
            }
        }

        MenuItem {
            id: exportProject
            text: qsTr("Export Current Project")
            onTriggered: {
                sofaApplication.currentProject.exportProject(sofaApplication.projectSettings.currentProject())
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

            function openDialog() {
                openSofaSceneDialog.open()
            }

//            Shortcut {
//                sequence: StandardKey.Open
//                context: Qt.ApplicationShortcut
//                onActivated: openMenuItem.openDialog()
//            }
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
            ListModel { id: scenesModel }
            property var modelList: sofaApplication.sceneSettings.sofaSceneRecents
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

        MenuItem {
            text: "Save";
            id: saveItem

            function saveScene()
            {
                var filePath = sofaApplication.sofaScene.path;
                sofaApplication.currentProject.saveScene(filePath.replace('file://', ''),
                                                         sofaApplication.sofaScene.root())
            }

            Shortcut {
                sequence: StandardKey.Save
                context: Qt.ApplicationShortcut
                onActivated: { saveItem.saveScene() }
            }
            onTriggered:  { saveItem.saveScene() }
        }
        MenuItem {
            text: "Save as..."

            FileDialog
            {
                id: fileDialog;
                visible: false
                selectExisting: false ///< indicate that the file dialog can be used to create new files.
                folder: sofaApplication.currentProject.rootDir.toString()
                onSelectionAccepted: {
                    sofaApplication.currentProject.saveScene(fileUrl.toString().replace('file://', ""), sofaApplication.sofaScene.root())
                }
            }

            Shortcut {
                sequence: StandardKey.SaveAs
                context: Qt.ApplicationShortcut
                onActivated: fileDialog.open()
            }
            onTriggered: fileDialog.open()
        }
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
                    console.log(o.item)
                    windowMenu.close()
                }
                ToolTip {
                    text: qsTr("Opens SOFA's store")
                    description: "The plugin store allows you to install additional plugins for your project"
                }
            }

            MenuSeparator {}

            Repeater {
                model: sofaApplication.sofaViewListModel
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
                    ProfilerView.open()
                    windowMenu.close()
                }
                ToolTip {
                    text: qsTr("Open the default Profiler")
                    description: "To visualize where the computation take place"
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
                color: sofaApplication.style.contentBackgroundColor

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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import QtQuick.Layouts 1.12
import Qt.labs.folderlistmodel 2.12
import Asset 1.0
import PythonAsset 1.0
import PythonAssetModel 1.0
import SofaColorScheme 1.0
import SofaWidgets 1.0
import QtQml 2.12
import QtQuick.Window 2.12

Menu {
    id: projectMenu

    property Asset model

    property var filePath
    property bool fileIsDir: true

    visible: false

    MenuItem {
        text: "Show Containing Folder"
        onTriggered: {
            projectMenu.visible = false
            sofaApplication.openInExplorer(filePath)
        }
    }
    MenuItem {
        id: openTerminal
        text: "Open Terminal Here"
        onTriggered: {
            projectMenu.visible = false
            sofaApplication.openInTerminal(filePath)
        }
    }
    
    MenuItem {
        id: newFolder
        text: "New Folder"
        
        onTriggered: {
            projectMenu.visible = false
            sofaApplication.createFolderIn(filePath)
        }
    }
    
    MenuItem {
        id: importAsset
        text: "Open asset store..."
        
        onTriggered: {
            var o = windowComponent.createObject(root, {
                                                     "source": "qrc:///SofaViews/WebBrowserView.qml",
                                                     "title" : "Sofa Ressources Repository",
                                                     "width" : 800,
                                                     "height": 600,
                                                 });
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
    
    Component {
        id: fileSpecificEntries
        MenuItem {
            id: openInEditor
            text: "Open In Editor"
            onTriggered: {
                projectMenu.visible = false
                sofaApplication.openInEditor(filePath)
            }
        }
    }
    Component {
        id: folderSpecificEntries
        MenuItem {
            id: openAsProject
            text: "Open Folder As Project"
            
            onTriggered: {
                projectMenu.visible = false
                sofaApplication.projectSettings.addRecent(filePath)
            }
        }
    }
    
    Component {
        id: sceneSpecificEntries
        MenuItem {
            id: projectFromScene
            text: "Load Scene"
            onTriggered: {
                projectMenu.visible = false
                sofaApplication.sofaScene.source = filePath
            }
            Component.onCompleted: {
                console.error("sceneSpecificEntries created")
            }
        }
    }
    
    Loader {
        sourceComponent: fileIsDir ? folderSpecificEntries: fileSpecificEntries
    }
    
    Loader {
        id: sceneEntriesLoader
        sourceComponent: (model && model.isScene) ? sceneSpecificEntries : null
    }
}

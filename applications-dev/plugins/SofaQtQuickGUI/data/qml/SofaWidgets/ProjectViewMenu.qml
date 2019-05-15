import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import QtQuick.Layouts 1.12
import Qt.labs.folderlistmodel 2.12
import AssetFactory 1.0
import SofaColorScheme 1.0
import SofaWidgets 1.0
import QtQml 2.12
import QtQuick.Window 2.12
//import AssetView 1.0

Menu {
    property var filePath
    property bool fileIsDir: true
    property bool fileIsScene: false

    id: projectMenu
    visible: false
    
    MenuItem {
        text: "Show Containing Folder"
        onTriggered: {
            projectMenu.visible = false
            console.error(filePath)
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
        text: "Open plugin store..."
        
        onTriggered: {
            var o = windowComponent.createObject(root, {
                                                     "source": "qrc:///SofaViews/WebBrowserView.qml",
                                                     "title" : "Sofa Asset Repository"
                                                 });
            o.scroll.webview.source = "http://www.google.fr"
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
        }
    }
    
    Loader {
        sourceComponent: fileIsDir ? folderSpecificEntries: fileSpecificEntries
    }
    
    Loader {
        id: sceneEntriesLoader
        sourceComponent: fileIsScene ? sceneSpecificEntries : null
    }
}

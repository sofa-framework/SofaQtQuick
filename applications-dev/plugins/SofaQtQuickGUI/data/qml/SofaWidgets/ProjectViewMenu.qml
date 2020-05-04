import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import QtQuick.Layouts 1.12
import Qt.labs.folderlistmodel 2.12
import Asset 1.0
import PythonAsset 1.0
import SofaColorScheme 1.0
import SofaWidgets 1.0
import QtQml 2.12
import QtQuick.Window 2.12
import SofaApplication 1.0

Menu {
    id: projectMenu

    property Asset model

    property var filePath
    property bool fileIsDir: true

    visible: false

    Component {
        id: fileSpecificEntries
        MenuItem {
            id: openInEditor
            text: "Open In Editor"
            onTriggered: {
                projectMenu.visible = false
                model.openThirdParty()
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
                SofaApplication.currentProject.rootDir = filePath
                SofaApplication.projectSettings.addRecent(SofaApplication.currentProject.rootDirPath)
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
                SofaApplication.sofaScene.source = filePath
            }
        }
    }

    Loader {
        id: sceneEntriesLoader
        sourceComponent: (model && model.isScene) ? sceneSpecificEntries : null
    }

    SofaAssetMenu {
        id: assetsList
        enabled: model && model.scriptContent.length > 1
        asset: model
        parentNode: SofaApplication.selectedComponent
        modal: true
    }

    MenuSeparator {}

    Loader {
        sourceComponent: fileIsDir ? folderSpecificEntries: fileSpecificEntries
    }


    MenuSeparator {}

    MenuItem {
        text: "Show Containing Folder"
        onTriggered: {
            projectMenu.visible = false
            SofaApplication.openInExplorer(filePath)
        }
    }
    MenuItem {
        id: openTerminal
        text: "Open Terminal Here"
        onTriggered: {
            projectMenu.visible = false
            SofaApplication.openInTerminal(filePath)
        }
    }
    
    MenuItem {
        id: newFolder
        text: "New Folder"
        
        onTriggered: {
            projectMenu.visible = false
            SofaApplication.createFolderIn(filePath.toString())
        }
    }
    Menu {
        id: newAsset
        title: "New..."

        MenuItem {
            text: "QMLUI Canvas"
            icon.source: "qrc:/icon/ICON_CANVAS2.png"
            onTriggered: {
                var file = SofaApplication.currentProject.createTemplateFile(filePath, "Canvas")
                SofaApplication.openInEditor(file)
            }
        }
        MenuItem {
            text: "Python Controller"
            icon.source: "qrc:/icon/ICON_CONTROLLER2.png"
            onTriggered: {
                var file = SofaApplication.currentProject.createTemplateFile(filePath, "Controller")
                SofaApplication.openInEditor(file)
            }
        }
        MenuItem {
            text: "Python ForceField"
            icon.source: "qrc:/icon/ICON_MAGNET.png"
            onTriggered: {
                var file = SofaApplication.currentProject.createTemplateFile(filePath, "ForceField")
                SofaApplication.openInEditor(file)
            }
        }
        MenuItem {
            text: "Python DataEngine"
            icon.source: "qrc:/icon/ICON_ENGINE3.png"
            onTriggered: {
                var file = SofaApplication.currentProject.createTemplateFile(filePath, "DataEngine")
                SofaApplication.openInEditor(file)
            }
        }
        MenuItem {
            text: "Python Prefab"
            icon.source: "qrc:/icon/ICON_PREFAB3.png"
            onTriggered: {
                var file = SofaApplication.currentProject.createTemplateFile(filePath, "Prefab")
                SofaApplication.openInEditor(file)
            }
        }
        MenuItem {
            text: "Scene"
            icon.source: "qrc:/icon/ICON_PYSCN_MONOCHROME.png"
            onTriggered: {
                var file = SofaApplication.currentProject.createTemplateFile(filePath, "Scene")
                SofaApplication.openInEditor(file)
                SofaApplication.currentProject.scan(file)
            }
        }
    }
    
    MenuItem {
        id: importAsset
        text: "Open store..."
        
        onTriggered: {
            var o = windowComponent.createObject(root, {
                                                     "source": "qrc:///SofaViews/WebBrowserView.qml",
                                                     "title" : "Sofa Ressources Repository",
                                                     "url" : "https://github.com/SofaDefrost/SPM/wiki/Sofa-Ressources",
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
                color: SofaApplication.style.contentBackgroundColor
                
                Loader {
                    id: loader
                    anchors.fill: parent
                    source: window.source
                }
            }
        }
    }

    MenuSeparator {}

    Loader {
        sourceComponent: !fileIsDir && model && model.extension !== "py" ? customizeAssetComponent : null
    }

    Component {
        id: customizeAssetComponent

            MenuItem {
            id: customizeAsset
            text: "Edit asset actions..."

            onTriggered: {
                var assetTemplateFile = SofaApplication.assetsDirectory() + model.extension + "Asset.py"
                SofaApplication.createAssetTemplate(assetTemplateFile);
                SofaApplication.openInEditor(assetTemplateFile);
            }

            ToolTip {
                text: "Open Asset template in editor"
                description: "Asset templates are scripts determining how assets are instantiated in the scene graph."
            }
        }
    }
}

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

Item {

    property var sofaApplication: null

    id: root

    anchors.fill : parent

    Item {
        id: self
        property var project: sofaApplication.currentProject
    }

    Rectangle {
        id: background
        anchors.fill : parent
        color: sofaApplication.style.contentBackgroundColor
    }

    ColumnLayout {
        anchors.fill: parent
        Rectangle {
            id: headerID
            Layout.fillWidth: true


            implicitHeight: 25
            color: sofaApplication.style.contentBackgroundColor
            border.color: "#3c3c3c"

            GBRect {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                implicitHeight: parent.implicitHeight - 1
                implicitWidth: parent.implicitWidth + 2
                borderWidth: 1
                borderGradient: Gradient {
                    GradientStop { position: 9.0; color: "#5c5c5c" }
                    GradientStop { position: 1.0; color: "#7a7a7a" }
                }
                color: "transparent"

                Label {
                    leftPadding: 10
                    text: folderModel.folder.toString().split("/")[folderModel.folder.toString().split("/").length -1]
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }


        ScrollView {
            id: scrollview

            Layout.fillWidth: true
            Layout.fillHeight: true
            ProjectViewMenu {
                id: generalProjectMenu
                filePath: folderModel.folder.toString().replace("file://", "")
                visible: false
            }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    generalProjectMenu.visible = !generalProjectMenu.visible
                    generalProjectMenu.x = mouse.x
                    generalProjectMenu.y = mouse.y
                }
            }

            ListView {
                id: folderView
                height: contentHeight
                width: parent.width
                header: RowLayout{
                    implicitWidth: folderView.width

                    Rectangle {
                        color: sofaApplication.style.contentBackgroundColor
                        Layout.fillWidth: true
                        implicitHeight: 20
                        implicitWidth: folderView.width / 3
                        Rectangle {
                            id: separator1
                            width: 2
                            height: 20
                            color: "#393939"
                            Rectangle {
                                x: 1
                                width: 1
                                height: 20
                                color: "#959595"
                            }
                        }
                        Label {
                            anchors.left: separator1.right
                            leftPadding: 5
                            color: "black"
                            text: "Name"
                        }
                    }

                    Rectangle {
                        color: sofaApplication.style.contentBackgroundColor
                        Layout.fillWidth: true
                        implicitHeight: 20
                        implicitWidth: folderView.width / 3
                        Rectangle {
                            id: separator2
                            width: 2
                            height: 20
                            color: "#393939"
                            Rectangle {
                                x: 1
                                width: 1
                                height: 20
                                color: "#959595"
                            }
                        }
                        Label {
                            anchors.left: separator2.right
                            leftPadding: 5
                            color: "black"
                            text: "Type"
                        }
                    }

                    Rectangle {
                        color: sofaApplication.style.contentBackgroundColor
                        Layout.fillWidth: true
                        implicitHeight: 20
                        implicitWidth: folderView.width / 3
                        Rectangle {
                            id: separator3
                            width: 2
                            height: 20
                            color: "#393939"
                            Rectangle {
                                x: 1
                                width: 1
                                height: 20
                                color: "#959595"
                            }
                        }
                        Label {
                            anchors.left: separator3.right
                            leftPadding: 5
                            color: "black"
                            text: "Size"
                        }
                    }
                }

                FolderListModel {
                    id: folderModel

                    property var projectDir: sofaApplication.projectSettings.recentProjects.split(";")[0]
                    onProjectDirChanged: {

                        self.project.rootDir = projectDir
                        folderModel.rootFolder = self.project.rootDir
                        folderModel.folder = self.project.rootDir
                    }

                    onFolderChanged: {
                        self.project.scanProject(folderModel.folder)
                    }

                    onDataChanged: {
                        // forces a rescan & refresh of the listview model
                        var tmp = folderModel.folder
                        folderModel.folder = folderModel.folder + "/.."
                        folderModel.folder = tmp
                    }

                    showDirs: true
                    showDotAndDotDot: true
                    showDirsFirst: true
                    showFiles: true
                    caseSensitive: true
                    folder: ""
                    nameFilters: self.project.assetFactory.getSupportedTypes()
                }

                delegate: Component {
                    id: fileDelegate
                    Rectangle {
                        id: wrapper
                        width: root.width
                        height: 20
                        color: index % 2 ? "#4c4c4c" : "#454545"
                        property string highlightColor: ListView.isCurrentItem ? "#82878c" : "transparent"
                        Rectangle {
                            anchors.fill: parent
                            color: wrapper.highlightColor
                            radius: 4
                            RowLayout {
                                anchors.fill: parent
                                Rectangle {
                                    Layout.fillWidth: true
                                    Image {
                                        id: iconId
                                        x: 5
                                        width: 15
                                        height: 15
                                        fillMode: Image.PreserveAspectFit
                                        source: fileIsDir ? "qrc:/icon/ICON_FILE_FOLDER.png" : self.project.assetFactory.getIcon(fileSuffix)
                                        anchors.verticalCenter: parent.verticalCenter
                                    }

                                    Text {
                                        width: parent.width
                                        leftPadding: 5
                                        text: fileName
                                        clip: true
                                        elide: Text.ElideRight
                                        color: (self.project.getAssetMetaInfo(fileURL)).toString() !== "" || fileIsDir ? "#efefef" : "darkgrey"
                                        anchors.left: iconId.right
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                                Rectangle {
                                    Layout.fillWidth: true
                                    Text {
                                        width: parent.width
                                        leftPadding: 10
                                        text: fileIsDir ? "Folder" : self.project.assetFactory.getTypeString(fileSuffix)
                                        color: (self.project.getAssetMetaInfo(fileURL)).toString() !== "" || fileIsDir ? "#efefef" : "darkgrey"
                                        clip: true
                                        elide: Text.ElideRight
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                                Rectangle {
                                    Layout.fillWidth: true
                                    Text {
                                        width: parent.width
                                        leftPadding: 10
                                        text: fileIsDir ? self.project.getFileCount(fileURL) :
                                                          (fileSize > 1e9) ? (fileSize / 1e9).toFixed(1) + " G" :
                                                                             (fileSize > 1e6) ? (fileSize / 1e6).toFixed(1) + " M" :
                                                                                                (fileSize > 1e3) ? (fileSize / 1e3).toFixed(1) + " k" :
                                                                                                                   fileSize + " bytes"
                                        color: (self.project.getAssetMetaInfo(fileURL)).toString() !== "" || fileIsDir ? "#efefef" : "darkgrey"
                                        clip: true
                                        elide: Text.ElideRight
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                            }
                        }

                        property bool isSceneFile: false

                        SofaAssetMenu {
                            id: assetMenu

                            model: self.project.getAssetMetaInfo(folderModel.get(index, "fileURL"))
                            visible: false
                        }

                        ProjectViewMenu {
                            id: projectMenu
                            filePath: folderModel.get(index, "filePath")
                            fileIsDir: index !== -1 ? folderModel.get(index, "fileIsDir") : ""
                            fileIsScene: {
                                var metadata = self.project.getAssetMetaInfo(folderModel.get(index, "fileURL"))
                                for (var m in metadata) {
                                    console.error(metadata[m].name)
                                    if (metadata[m].name === "createScene")
                                        return true
                                }
                                return false
                            }
                        }

                        MouseArea {
                            id: mouseRegion
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            anchors.fill: parent
                            hoverEnabled: true
                            onHoveredChanged: {
                                if (containsMouse) {
                                    folderView.currentIndex = index;
                                }
                            }
                            onDoubleClicked: {
                                if (folderModel.isFolder(index)) {
                                    folderModel.folder = folderModel.get(index, "fileURL")
                                } else {

                                    if (wrapper.isSceneFile) {
                                        sofaApplication.sofaScene.source = folderModel.get(index, "filePath")
                                    }
                                    else {
                                        var rootNode = sofaApplication.sofaScene.root()
                                        insertAsset(index, rootNode)
                                    }

                                }

                            }
                            onClicked: {
                                if (Qt.RightButton === mouse.button)
                                {
                                    projectMenu.visible = true
                                    //                                    // Let's load detailed info if available
                                    //                                    if (!folderModel.isFolder(index))
                                    //                                    {
                                    //                                        assetMenu.visible = true
                                    //                                    }
                                }
                            }

                            drag.target: draggedData

                            drag.onActiveChanged: {
                                if (drag.active) {
                                    draggedData.ctxMenu = assetMenu

                                    draggedData.url = folderModel.get(index, "fileURL")
                                    var s = draggedData.url.toString()
                                    var newString = s.replace(self.project.rootDir, "");
                                    draggedData.localPath = newString
                                } else {
                                    console.error("Drag Finished")

                                }
                            }
                            function insertAsset(index, rootNode) {
                                var component = self.project.getAsset(folderModel.get(index, "fileURL"))
                                console.error("Parent node for asset is " + rootNode)
                                component.copyTo(rootNode)
                            }

                            Item {
                                id: draggedData
                                Drag.active: self.project.getAssetMetaInfo(fileURL).toString() !== "" || fileIsDir ? mouseRegion.drag.active : false
                                Drag.dragType: Drag.Automatic
                                Drag.supportedActions: Qt.CopyAction
                                Drag.mimeData: {
                                    "text/plain": "Copied text"
                                }
                                property string origin: "ProjectView"
                                property bool accepted: false
                                property point beginDrag
                                property var node
                                property var ctxMenu
                                property url url
                                property string localPath

                                function getAsset(assetName) {
                                    return self.project.getAsset(folderModel.get(index, "fileURL"), assetName)
                                }
                            }
                        }
                    }
                }

                model: folderModel
                highlight: Rectangle { color: "#82878c"; radius: 5 }
                focus: true
            }
        }
    }


}

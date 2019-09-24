import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import QtQuick.Layouts 1.12
import Qt.labs.folderlistmodel 2.12
import SofaColorScheme 1.0
import SofaWidgets 1.0
import QtQml 2.12
import QtQuick.Window 2.12
import Asset 1.0
import PythonAsset 1.0
import TextureAsset 1.0
import MeshAsset 1.0

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
        anchors.fill: root.parent
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

            width: root.parent.width
            height: root.parent.height - 42
            ProjectViewMenu {
                id: generalProjectMenu
                filePath: folderModel.folder.toString().replace("file://", "")
                visible: false
            }
            MouseArea {
                id: projectViewMouseArea
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    var pos = sofaApplication.getIdealPopupPos(generalProjectMenu, projectViewMouseArea)
                    generalProjectMenu.x = mouse.x + pos[0]
                    generalProjectMenu.y = mouse.y + pos[1]
                    generalProjectMenu.open()
                }
            }
            ListView {
                id: folderView
                anchors.fill: parent
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                clip: true

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

                    showDirs: true
                    showDotAndDotDot: true
                    showDirsFirst: true
                    showFiles: true
                    caseSensitive: true
                    folder: ""
                    nameFilters: self.project.getSupportedTypes()
                }

                property var selectedItem: null
                delegate: Component {
                    id: fileDelegate

                    Rectangle {
                        id: wrapper
                        property var asset: self.project.getAsset(filePath)

                        width: root.width
                        height: 20
                        color: index % 2 ? "#4c4c4c" : "#454545"
                        property string highlightColor: ListView.isCurrentItem || folderView.selectedItem === wrapper ? "#82878c" : "transparent"
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
                                        source: fileIsDir ? "qrc:/icon/ICON_FILE_FOLDER.png" : wrapper.asset.iconPath
                                        anchors.verticalCenter: parent.verticalCenter
                                    }

                                    Text {
                                        width: parent.width
                                        leftPadding: 5
                                        text: fileName
                                        clip: true
                                        elide: Text.ElideRight
                                        color: fileIsDir || wrapper.asset.isSofaContent ? "#efefef" : "darkgrey"
                                        anchors.left: iconId.right
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                                Rectangle {
                                    Layout.fillWidth: true
                                    Text {
                                        width: parent.width
                                        leftPadding: 10
                                        text: fileIsDir ? "Folder" : asset.typeString
                                        color: fileIsDir || wrapper.asset.isSofaContent ? "#efefef" : "darkgrey"
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
                                        color: fileIsDir || wrapper.asset.isSofaContent ? "#efefef" : "darkgrey"
                                        clip: true
                                        elide: Text.ElideRight
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                            }
                        }

                        property bool isSceneFile: false

                        ProjectViewMenu {
                            id: projectMenu
                            filePath: folderModel.get(index, "filePath")
                            fileIsDir: index !== -1 ? folderModel.get(index, "fileIsDir") : ""
                            model: folderModel.get(index, "fileIsDir") ? null : self.project.getAsset(filePath)
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

                            function insertAsset(index, rootNode)
                            {
                                var newNode = self.project.getAsset(folderModel.get(index, "filePath")).create()
                                var _parent = sofaScene.selectedComponent
                                if (_parent === null) { console.error("taking root node"); _parent = sofaScene.root()}
                                if (!_parent.isNode()) { console.error("taking object's parent"); _parent = _parent.getFirstParent()}

                                var hasNodes = newNode.getChildren().size()
                                console.error("ParentNode type: " + _parent)
                                console.error("newNode type: " + newNode)
                                _parent.dump()
                                newNode.copyTo(_parent)
                                console.error("bah shit")
                                if (hasNodes) {
                                    var childsList = _parent.getChildren()
                                    if (childsList.size() !== 0) {
                                        return childsList.last()
                                    }
                                }
                                return parent
                            }

                            onDoubleClicked: {
                                if (folderModel.isFolder(index)) {
                                    folderModel.folder = folderModel.get(index, "fileURL")
                                } else {
                                    if (self.project.getAsset(folderModel.get(index, "filePath")).isScene) {
                                        sofaApplication.sofaScene.source = folderModel.get(index, "filePath")
                                    }
                                    else {
                                        var rootNode = sofaApplication.sofaScene.root()
                                        sofaScene.selectedComponent = insertAsset(index, rootNode)
                                    }
                                }
                            }
                            onClicked: {
                                if (Qt.RightButton === mouse.button)
                                {
                                    var pos = sofaApplication.getIdealPopupPos(projectMenu, mouseRegion)
                                    projectMenu.x = mouse.x + pos[0]
                                    projectMenu.y = mouse.y + pos[1]
                                    console.error(projectMenu.x + " " + projectMenu.y)
                                    projectMenu.open()
                                }
                                else if (Qt.LeftButton === mouse.button)
                                {
                                    folderView.selectedItem = wrapper
                                    wrapper.asset = self.project.getAsset(folderModel.get(index, "filePath"))
                                    sofaApplication.currentProject.selectedAsset = wrapper.asset;
                                }
                            }

                            drag.target: draggedData

                            drag.onActiveChanged: {
                                if (drag.active)
                                    draggedData.asset = wrapper.asset
                            }

                            Item {
                                id: draggedData
                                Drag.active: !fileIsDir ? mouseRegion.drag.active : false
                                Drag.dragType: Drag.Automatic
                                Drag.supportedActions: Qt.CopyAction
                                Drag.mimeData: {
                                    "text/plain": "Copied text"
                                }
                                property string origin: "ProjectView"
                                property Asset asset
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

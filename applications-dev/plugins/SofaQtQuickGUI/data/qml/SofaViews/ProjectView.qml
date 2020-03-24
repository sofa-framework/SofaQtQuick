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
import QtGraphicalEffects 1.12
import SofaApplication 1.0

Item {
    property bool isDebugPrintEnabled: SofaApplication.currentProject.isDebugPrintEnabled

    onIsDebugPrintEnabledChanged: {
        SofaApplication.currentProject.isDebugPrintEnabled = isDebugPrintEnabled
    }

    id: root
    anchors.fill : parent

    Rectangle {
        id: background
        anchors.fill : parent
        color: SofaApplication.style.contentBackgroundColor
    }


    ColumnLayout {
        anchors.fill: parent
        Rectangle {
            id: headerID
            Layout.fillWidth: true
            MouseArea {
                anchors.fill: parent
                onPressed: {
                    root.forceActiveFocus()
                }
            }

            implicitHeight: 25
            color: SofaApplication.style.contentBackgroundColor
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
                    text: {
                        var path = folderModel.folder.toString().replace("file://", "").replace("qrc:", "");
                        if (path.split("/")[path.split("/").length -1] === ".")
                            return path.slice(0, path.lastIndexOf("/"))
                        else if (path.split("/")[path.split("/").length -1] === "..") {
                            var arr = path.split("/")
                            arr.pop();
                            arr.pop();
                            return arr.join("/")
                        }
                        else
                            return path;
                    }
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }


        Flickable {
            id: scrollview

            Layout.fillHeight: true
            Layout.fillWidth: true

            ProjectViewMenu {
                id: generalProjectMenu
                filePath: folderModel.folder.toString().replace("file://", "").replace("qrc:", "")
                visible: false
            }
            MouseArea {
                id: projectViewMouseArea
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    forceActiveFocus()
                    var pos = SofaApplication.getIdealPopupPos(generalProjectMenu, projectViewMouseArea)
                    generalProjectMenu.x = mouse.x + pos[0]
                    generalProjectMenu.y = mouse.y + pos[1]
                    generalProjectMenu.open()
                }
            }
            ListView {
                id: folderView
                interactive: true
                focus: true
                anchors.fill: parent
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                clip: true
                flickableDirection: Flickable.VerticalFlick
                boundsMovement: Flickable.StopAtBounds
                ScrollBar.horizontal: ScrollBar {
                    policy: ScrollBar.AlwaysOff
                }

                ScrollBar.vertical: VerticalScrollbar {
                    content: folderView.contentItem
                }

                header: Rectangle {
                    height: 18
                    implicitWidth: folderView.width
                    color: "transparent"
                    MouseArea {
                        width: childrenRect.width
                        height: childrenRect.height
                        onPressed: {
                            root.forceActiveFocus()
                        }
                        RowLayout {
                            Rectangle {
                                color: SofaApplication.style.contentBackgroundColor
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
                                color: SofaApplication.style.contentBackgroundColor
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
                                color: SofaApplication.style.contentBackgroundColor
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
                    }
                }
                FolderListModel {
                    id: folderModel

                    property var projectDir: SofaApplication.currentProject.rootDirPath
                    onProjectDirChanged: {
                        folderModel.rootFolder = Qt.binding(function(){ return SofaApplication.currentProject.rootDirPath; })
                        folderModel.folder = SofaApplication.currentProject.rootDirPath
                    }

                    showDirs: true
                    showDotAndDotDot: true
                    showDirsFirst: true
                    showFiles: true
                    caseSensitive: true
                    rootFolder: SofaApplication.currentProject.rootDirPath
//                    nameFilters: SofaApplication.currentProject.getSupportedTypes()
                }

                property var selectedItem: null
                delegate: Component {
                    id: fileDelegate

                    Rectangle {
                        id: wrapper
                        width: root.width
                        height: 20
                        color: index % 2 ? "#4c4c4c" : "#454545"
                        property string highlightColor: ListView.isCurrentItem || folderView.selectedItem === wrapper ? "#82878c" : "transparent"
                        property var asset: SofaApplication.currentProject.getAsset(filePath)
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
                                        source: fileIsDir ? "qrc:/icon/ICON_FILE_FOLDER.png" : asset ? asset.iconPath : "qrc:/icon/ICON_FILE_BLANK.png"
                                        anchors.verticalCenter: parent.verticalCenter
                                    }

                                    Text {
                                        width: parent.width
                                        leftPadding: 5
                                        text: fileName
                                        clip: true
                                        elide: Text.ElideRight
                                        color: fileIsDir || (asset ? asset.isSofaContent :false) ? "#efefef" : "darkgrey"
                                        anchors.left: iconId.right
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                                Rectangle {
                                    Layout.fillWidth: true
                                    Text {
                                        width: parent.width
                                        leftPadding: 10
                                        text: fileIsDir ? "Folder" : asset ? asset.typeString : "Unknown file type"
                                        color: fileIsDir || (asset ? asset.isSofaContent : false) ? "#efefef" : "darkgrey"

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
                                        text: fileIsDir ? SofaApplication.currentProject.getFileCount(fileURL) :
                                                          (fileSize > 1e9) ? (fileSize / 1e9).toFixed(1) + " G" :
                                                                             (fileSize > 1e6) ? (fileSize / 1e6).toFixed(1) + " M" :
                                                                                                (fileSize > 1e3) ? (fileSize / 1e3).toFixed(1) + " k" :
                                                                                                                   fileSize + " bytes"
                                        color: fileIsDir || (asset ? asset.isSofaContent : false) ? "#efefef" : "darkgrey"
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
                            model: folderModel.get(index, "fileIsDir") ? null : asset
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

                            function insertAsset(index)
                            {
                                var _parent = SofaApplication.selectedComponent
                                if (_parent === null) { console.error("taking root node"); _parent = sofaScene.root()}
                                if (!_parent.isNode()) { console.error("taking object's parent"); _parent = _parent.getFirstParent()}

                                var newNode = SofaApplication.currentProject.getAsset(folderModel.get(index, "filePath")).create(_parent)
                                var hasNodes = newNode.children().size()
                                console.error("ParentNode type: " + _parent)
                                console.error("newNode type: " + newNode)
                                _parent.dump()
                                //                                newNode.copyTo(_parent)
                                if (hasNodes) {
                                    var childsList = _parent.children()
                                    if (childsList.size() !== 0) {
                                        return childsList.last()
                                    }
                                }
                                return _parent
                            }

                            onDoubleClicked: {
                                forceActiveFocus()
                                if (folderModel.isFolder(index)) {
                                    folderModel.folder = folderModel.get(index, "fileURL")
                                } else {
                                    if (SofaApplication.currentProject.getAsset(folderModel.get(index, "filePath")).isScene) {
                                        SofaApplication.sofaScene.source = folderModel.get(index, "filePath")
                                    }
                                    else {
                                        var insertedAsset = insertAsset(index)
                                        SofaApplication.selectedComponent = insertedAsset
                                    }
                                }
                            }
                            onClicked: {
                                forceActiveFocus()
                                if (Qt.RightButton === mouse.button)
                                {
                                    var pos = SofaApplication.getIdealPopupPos(projectMenu, mouseRegion)
                                    projectMenu.x = mouse.x + pos[0]
                                    projectMenu.y = mouse.y + pos[1]
                                    projectMenu.open()
                                }
                                else if (Qt.LeftButton === mouse.button)
                                {
                                    folderView.selectedItem = wrapper
                                    SofaApplication.currentProject.selectedAsset = SofaApplication.currentProject.getAsset(filePath);
                                }
                            }

                            drag.target: draggedData
                            drag.onActiveChanged: {
                                if (drag.active)
                                    draggedData.asset = SofaApplication.currentProject.getAsset(filePath)
                            }

                            DraggableAssetItem {
                                id: draggedData

                                origin: "ProjectView"
                                Drag.active: !fileIsDir ? mouseRegion.drag.active : false
                            }
                        }
                    }
                }

                model: folderModel
                highlight: Rectangle { color: "#82878c"; radius: 5 }
            }
        }
    }
}

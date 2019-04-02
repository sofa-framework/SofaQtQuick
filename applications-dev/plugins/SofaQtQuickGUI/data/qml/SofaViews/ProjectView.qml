import QtQuick 2.12
import QtQuick.Controls 2.12
import SofaBasics 1.0
import QtQuick.Layouts 1.12
import Qt.labs.folderlistmodel 2.12
import AssetFactory 1.0
import SofaColorScheme 1.0
import AssetView 1.0

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

            ListView {
                id: folderView
                anchors.fill: parent
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
                    nameFilters: ["*"]
                }

                Component {
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
                                        x: 5
                                        id: iconId
                                        source: fileIsDir ? "qrc:/icon/ICON_FILE_FOLDER.png" : self.project.assetFactory.getIcon(fileSuffix)
                                        anchors.verticalCenter: parent.verticalCenter
                                    }

                                    Text {
                                        width: parent.width
                                        leftPadding: 5
                                        text: fileName
                                        clip: true
                                        elide: Text.ElideRight
                                        color: "#efefef"
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
                                        color: "#efefef"
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
                                                          (fileSize < 1e3) ? fileSize + " bytes" :
                                                                             (fileSize < 1e6) ? (fileSize / 1e3).toFixed(1) + " MB" :
                                                                                                (fileSize < 1e9) ? (fileSize / 1e6).toFixed(1) + " GB" :
                                                                                                                   "File too large"
                                        color: "#efefef"
                                        clip: true
                                        elide: Text.ElideRight
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                            }
                        }

                        MouseArea {
                            id: mouseRegion
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            anchors.fill: parent
                            hoverEnabled: true;
                            onHoveredChanged: {
                                if (containsMouse) {
                                    folderView.currentIndex = index;
                                }
                            }
                            onClicked: {
                                if (Qt.LeftButton === mouse.button)
                                {
                                    if (folderModel.isFolder(index))
                                    {
                                        folderModel.folder = folderModel.get(index, "fileURL")
                                    }
                                }
                                else
                                {
                                }
                            }
                        }
                    }
                }

                model: folderModel
                delegate: fileDelegate
                highlight: Rectangle { color: "#82878c"; radius: 5 }
                focus: true
            }
        }


        GroupBox {
            id: previewGroupID
            Layout.fillWidth: true
            implicitHeight: contentHeight ? contentHeight : 200
            height: implicitHeight

            title: "Preview"

            AssetView {
                id: assetView
                anchors.fill: parent
                drawFrame: false
            }

        }
    }


}

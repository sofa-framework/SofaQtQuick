import QtQuick 2.12
import QtQuick.Controls 2.12
import SofaBasics 1.0
import QtQuick.Layouts 1.12
import Qt.labs.folderlistmodel 2.12

Item {

    property var sofaApplication: null

    id: root

    anchors.fill : parent

    Rectangle {
        id: background
        anchors.fill : parent
        color: sofaApplication.style.contentBackgroundColor
    }
    ScrollView {
        id: scrollview
        clip: true
        height: parent.height - previewGroupID.height
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right

        ListView {
            id: folderView
            anchors.fill: parent
            header: Label {
                text: folderModel.folder
            }

            FolderListModel {
                id: folderModel

                property var recentProjects: sofaApplication.projectSettings.recentProjects
                onRecentProjectsChanged: {
                    folderModel.folder = sofaApplication.projectSettings.currentProject()
                    console.log(folderModel.count)
                }

                showDirs: true
                showDotAndDotDot: true
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
                    color: ListView.isCurrentItem ? "#82878c" :  index % 2 ? sofaApplication.style.contentBackgroundColor : sofaApplication.style.alternateBackgroundColor
                    Text {
                        text: fileName
                        color: "#efefef"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    MouseArea {
                        id: mouseRegion
                        anchors.fill: parent
                        hoverEnabled: true;
                        onHoveredChanged: {
                            if (containsMouse) {
                                folderView.currentIndex = index;
                            }
                        }
                        onClicked: {
                            if (folderModel.isFolder(index))
                            {
                                console.log(index)
                                folderModel.folder = folderModel.get(index, "fileURL")
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
        title: "Preview"
        Image {
            source: "qrc:/icon/sofaLogo.png"
        }
//        anchors.top: scrollview.bottom
        anchors.bottom: parent.bottom
    }
}

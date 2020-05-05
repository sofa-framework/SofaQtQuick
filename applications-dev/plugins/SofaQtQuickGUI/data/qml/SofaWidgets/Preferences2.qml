import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import SofaBasics 1.0
import SofaApplication 1.0

Window {
    id: preferences
    title: "Options"
    width: 800
    height: 600
    color: "#2a2b2c"
    RowLayout {
        anchors.margins: 20
        anchors.fill: parent
        Rectangle {
            Layout.minimumWidth: 180
            Layout.maximumWidth: 180
            Layout.fillHeight: true
            color: "transparent"
            border.color: "black"
            border.width: 1
            ListView {
                id: listview
                anchors.fill: parent
                model: ListModel {
                    ListElement {
                        name: "Config"
                        icon: "qrc:/icon/ICON_CAMERA.png"
                        component: "qrc:/SofaWidgets/ConfigPreferences.qml"
                    }
                    ListElement {
                        name: "Templates"
                        icon: "qrc:/icon/ICON_FILE_BLANK.png"
                        component: "qrc:/SofaWidgets/TemplatesPreferences.qml"
                    }
                    ListElement {
                        name: "External tools"
                        icon: "qrc:/icon/ICON_MAGNET.png"
                        component: "qrc:/SofaWidgets/ExternalToolsPreferences.qml"
                    }
                }
                delegate: Rectangle {
                    id: delegate
                    color: "transparent"
                    height: 30

                    Image {
                        id: image
                        anchors.verticalCenter: parent.verticalCenter
                        fillMode: Image.PreserveAspectFit
                        source: icon
                        x: 10
                    }
                    Text {
                        anchors.left: image.right
                        anchors.margins: 10
                        anchors.verticalCenter: parent.verticalCenter
                        text: name
                        color: "black"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            print("pouet")
                            loader.source = component
                            listview.currentIndex = index
                        }
                    }
                }

                highlightMoveDuration: 0
                highlight: Rectangle {
                    width: 180
                    height: 30
                    color: "#26525d"
                }
            }
        }
        Rectangle {
            Rectangle {
                anchors.fill: parent
                anchors.margins: 1
                color: "transparent"
                border.color: "#656565"
                border.width: 1
            }
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#4f4f50"
            border.color: "black"
            border.width: 1

            Loader {
                id: loader
                anchors.fill: parent
                anchors.margins: 10

            }
        }
    }
}

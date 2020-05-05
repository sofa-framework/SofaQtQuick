import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.4 as QQC1
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
                anchors.margins: 1
                currentIndex: 0
                model: ListModel {
                    ListElement {
                        name: "Config"
                        icon: "qrc:/icon/ICON_PARAMS.png"
                        component: "qrc:/SofaWidgets/ConfigPreferences.qml"
                    }
                    ListElement {
                        name: "Templates"
                        icon: "qrc:/icon/ICON_FILE_BLANK.png"
                        component: "qrc:/SofaWidgets/TemplatesPreferences.qml"
                    }
                    ListElement {
                        name: "External tools"
                        icon: "qrc:/icon/ICON_FILE_BLANK.png"
                        component: "qrc:/SofaWidgets/ExternalToolsPreferences.qml"
                    }
                }
                delegate: Rectangle {
                    color: "transparent"

                    width: 178
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
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        text: name
                        font.bold: true
                        color: "#ABABAB"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            loader.source = component
                            listview.currentIndex = index
                        }
                    }
                }

                highlightMoveDuration: 0
                highlight: Rectangle {
                    color: "#26525d"
                }
            }
        }
        Rectangle {
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true

            Loader {
                id: loader
                anchors.fill: parent
                anchors.margins: 10
                source: listview.model.get(listview.currentIndex).component
            }
        }
    }
}

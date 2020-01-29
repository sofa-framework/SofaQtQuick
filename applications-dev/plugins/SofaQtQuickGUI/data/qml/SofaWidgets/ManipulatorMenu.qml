import QtQuick 2.0
import QtQuick.Controls 2.0
import SofaBasics 1.0

Rectangle {
    id: root
    property var manipulator

    property var option
    property var image: ""
    property alias containsMouse: marea.containsMouse
    property var model: ListModel {
        id: menuItemModel
        ListElement {
            title: "Local"
            option: true
            iconSource: ""
        }
        ListElement {
            title: "Global"
            option: false
            iconSource: ""
        }
    }

    width: 30
    height: 30
    opacity: 1.0
    color: manipulator.enabled ? "#5680c2" : marea.containsMouse ? "#686868" : "#525252"

    Image {
        anchors.centerIn: parent
        source: image
        scale: 1.2
        opacity: 0.9
    }

    MouseArea {
        id: marea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            if (!root.model)
                root.option = !option
            else
            {
                manipulatorMenu.open()
            }
        }
        Menu {
            id: manipulatorMenu
            x: 30
            implicitWidth: 80
            title: "plop"
            style: "overlay"
            Repeater {
                id: repeater
                model: root.model

                MenuItem {
                    text: title
                    style: "overlay"
                    icon.source: iconSource
                    onTriggered: {
                        root.option = option
                    }
                }
            }
        }
    }
}


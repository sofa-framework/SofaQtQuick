import QtQuick 2.0
import QtQuick.Controls 2.0
import SofaBasics 1.0

Rectangle {
    id: root
    property var option
    property var image: ""
    property bool selected
    property alias containsMouse: marea.containsMouse
    property var model: ListModel {
        id: menuItemModel
        ListElement {
            title: "Local"
            option: true
        }
        ListElement {
            title: "Global"
            option: false
        }
    }

    width: 30
    height: 30
    opacity: 1.0
    color: selected ? "#5680c2" : marea.containsMouse ? "#686868" : "#525252"
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
            implicitWidth: 60
            title: "plop"
            style: "overlay"
            Repeater {
                id: repeater
                model: root.model

                MenuItem {
                    text: title
                    style: "overlay"
                    onTriggered: {
                        root.option = option
                    }
                }
            }
        }
    }
}

    //Rectangle {
    //    id: root
    //    property var image: ""
    //    property var menuItemModel: ListModel {
    //        id: menuItemModel
    //        ListElement {
    //            title: "Local"
    //            option: true
    //        }
    //        ListElement {
    //            title: "Global"
    //            option: false
    //        }
    //    }
    //    signal optionSelected(var option)
    //    property bool selected
    //    onSelectedChanged: {
    //        menuItems.visible = selected
    //    }


    //    property alias containsMouse: mousemenu.containsMouse
    //    width: 30
    //    height: 30
    //    color: "transparent"
    //    Rectangle {
    //        id: menu
    //        anchors.fill: parent
    //        color: "transparent"
    //        Rectangle {
    //            id: menuBackground
    //            anchors.fill: parent
    //            color: root.selected || mousemenu.isPressed ? "#8888ff" : "white"
    //            opacity: root.selected || mousemenu.isPressed ? 0.7 : mousemenu.containsMouse ? 0.2 : 0.1
    //        }

    //        Image {
    //            anchors.centerIn: parent
    //            source: image
    //            scale: 1.2
    //            opacity: 0.9
    //        }
    //        Image {
    //            anchors.right: parent.right
    //            anchors.bottom: parent.bottom
    //            anchors.rightMargin: -3
    //            anchors.bottomMargin: -3
    //            source: "qrc:/icon/ManipulatorMenuCorner.png"
    //            scale: 0.35
    //            opacity: 0.7
    //        }

    //        MouseArea {
    //            id: mousemenu
    //            anchors.fill: parent
    //            hoverEnabled: true
    //            function toggleMenuItems() {
    //                menuItems.visible = !menuItems.visible
    //            }
    //            onClicked: {
    //                toggleMenuItems()
    //            }
    //        }
    //    }

    //    Column {
    //        id: menuItems
    //        visible: false
    //        anchors.left: menu.right
    //        anchors.top: menu.top

    //        Repeater {
    //            id: repeater
    //            model: root.menuItemModel
    //            Rectangle {
    //                id: menuItemDelegate
    //                height: 30
    //                width: 100
    //                color: "transparent"
    //                Rectangle {
    //                    anchors.fill: parent
    //                    color: mouseMenuItem.containsMouse ? "#8888ff" : "white"
    //                    opacity: mouseMenuItem.containsMouse ? 0.7 : mousemenu.containsMouse ? 0.2 : 0.1
    //                }

    //                Label {
    //                    x: 10
    //                    text: title
    //                    color: "white"
    //                    anchors.verticalCenter: parent.verticalCenter
    //                }
    //                MouseArea {
    //                    anchors.fill: parent
    //                    id: mouseMenuItem
    //                    hoverEnabled: true
    //                    onClicked: {
    //                        console.log("clicked")
    //                        root.selected = false
    //                        root.optionSelected(option)
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 1.4
import SofaBasics 1.0
import SofaApplication 1.0

TabView {
    id: root
    Tab {
        title: "General"
        anchors.margins: 10
        GroupBox {
            title: "Location:"
            RowLayout {
                anchors.fill: parent
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    border.color: "black"
                    color: "#2a2b2c"

                    ListView {
                        id: listview
                        anchors.fill: parent

                        anchors.margins: 1
                        currentIndex: 0
                        model: ListModel {
                            ListElement {
                                path: "%ProjectDir%/config/"
                            }
                            ListElement {
                                path: "~/.config/runSofa2/"
                            }
                            ListElement {
                                path: "%SOFA_INSTALL_DIR%/bin/config/"
                            }
                        }
                        delegate: Rectangle {
                            id: rect
                            color: index === listview.currentIndex ? "#26525d" : "#2a2b2c"
                            width: parent.width
                            height: 20
                            Text {
                                x: 10
                                anchors.verticalCenter: parent.verticalCenter
                                text: path
                                font.bold: true
                                color: "#ABABAB"
                            }
                            MouseArea {
                                id: mouseArea
                                Menu {
                                    id: contextMenu
                                    visible: false
                                    MenuItem {
                                        text: "Copy full path"
                                        enabled: false
                                    }
                                    MenuItem {
                                        text: "Open terminal here"
                                        onTriggered: {
                                            SofaApplication.openInTerminal(path)
                                        }
                                    }
                                    MenuItem {
                                        text: "Show containing folder"
                                        onTriggered: {
                                            SofaApplication.openInExplorer(path)
                                        }
                                    }
                                }

                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: {
                                    listview.currentIndex = index
                                    if (mouse.button === Qt.RightButton)
                                    {
                                        forceActiveFocus()
                                        var pos = SofaApplication.getIdealPopupPos(contextMenu, mouseArea)
                                        contextMenu.x = mouse.x + pos[0]
                                        contextMenu.y = mouse.y + pos[1]
                                        contextMenu.open()
                                    }
                                }
                            }
                        }
                    }
                }
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.minimumWidth: 100
                    spacing: 2
                    Button {
                        implicitWidth: parent.width
                        text: "Add"
                    }
                    Button {
                        implicitWidth: parent.width
                        text: "Remove"
                    }
                    Rectangle {
                        width: 1
                        height: 10
                        color: "transparent"
                    }

                    Button {
                        implicitWidth: parent.width
                        text: "Move up"
                    }
                    Button {
                        implicitWidth: parent.width
                        text: "Move down"
                    }

                    Rectangle {
                        width: 1
                        Layout.fillHeight: true
                        color: "transparent"
                    }
                    Button {
                        implicitWidth: parent.width
                        text: "Reset defaults"
                    }

                }
            }
        }
    }
}

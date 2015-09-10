import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import SceneListModel 1.0

CollapsibleGroupBox {
    id: root
    implicitWidth: 0

    title: "Scene Graph"
    property int priority: 90

    property var scene: null
    property bool activatedData: true
    property real rowHeight: 16

    enabled: scene ? scene.ready : false

    ScrollView {
        anchors.fill: parent
        implicitHeight: Math.min(listView.implicitHeight, listView.contentHeight + 15)

        ListView {
            id: listView
            implicitHeight: 400
            clip: true

            model: SceneListModel {
                id: listModel
                scene: root.scene

                property bool dirty: true
            }

            Connections {
                target: root.scene
                onStatusChanged: {
                    listView.currentIndex = -1;
                }
                onStepEnd: {
                    if(root.scene.play)
                        listModel.dirty = true;
                    else if(listModel)
                        listModel.update();
                }
                onReseted: {
                    if(listModel)
                        listModel.update();
                }
                onSelectedComponentChanged: {
                    listView.currentIndex = listModel.getComponentId(scene.selectedComponent);
                }
            }

            Timer {
                running: root.scene.play ? true : false
                repeat: true
                interval: 200
                onTriggered: {
                    if(listModel.dirty) {
                        listModel.update()
                        listModel.dirty = false;
                    }
                }
            }

            focus: true

            onCurrentIndexChanged: {
                scene.selectedComponent = listModel.getComponentById(listView.currentIndex);
            }

            onCountChanged: {
                if(currentIndex >= count)
                    currentIndex = -1;
            }

            highlightMoveDuration: 0
            highlight: Rectangle {
                color: "lightsteelblue";
                radius: 5
            }

            delegate: Item {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: depth * rowHeight
                height: visible ? rowHeight : 0
                visible: !(SceneListModel.Hidden & visibility)

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    Item {
                        Layout.preferredHeight: rowHeight
                        Layout.preferredWidth: Layout.preferredHeight

                        Image {
                            anchors.fill: parent
                            visible: collapsible
                            source: !(SceneListModel.Collapsed & visibility) ? "qrc:/icon/downArrow.png" : "qrc:/icon/rightArrow.png"

                            MouseArea {
                                anchors.fill: parent
                                enabled: collapsible
                                onClicked: listView.model.setCollapsed(index, !(SceneListModel.Collapsed & visibility))
                            }
                        }
                    }

                    Text {
                        text: (multiparent ? "Multi" : "") + (0 !== type.length || 0 !== name.length ? type + " - " + name : "")
                        color: Qt.darker(Qt.rgba((depth * 6) % 9 / 8.0, depth % 9 / 8.0, (depth * 3) % 9 / 8.0, 1.0), 1.5)
                        font.bold: isNode

                        Layout.fillWidth: true
                        Layout.preferredHeight: rowHeight

                        // Menu to activate or desactivate a node
                        Menu {
                            id: nodeMenu
                            MenuItem {
                                text: {
                                    activatedData ? "Desactivate" :"Activate"
                                }
                                onTriggered: {
                                    activatedData ? d.sceneData.setValue(0) : d.sceneData.setValue(1)
                                    activatedData = d.sceneData.value()
                                    // Hide/Show children of desactivated/activated node
                                    listView.model.setCollapsed(index, !(SceneListModel.Collapsed & visibility))
                                    // Change color of current item
                                    activatedData ? listView.currentItem.opacity = 1 : listView.currentItem.opacity = 0.2
                                }
                            }
                        }
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            onClicked: {
                                if (mouse.button == Qt.LeftButton) {
                                    listView.currentIndex = index
                                }
                                else if (mouse.button == Qt.RightButton && isNode) {
                                    listView.currentIndex = index
                                    d.sceneData = scene.selectedComponent.getComponentData("activated")
                                    activatedData = d.sceneData.value()
                                    nodeMenu.popup()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    QtObject {
        id : d

        property QtObject sceneData
    }
}

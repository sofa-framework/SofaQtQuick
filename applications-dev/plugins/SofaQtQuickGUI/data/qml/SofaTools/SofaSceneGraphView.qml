/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0

CollapsibleGroupBox {
    id: root
    implicitWidth: 0

    title: "Sofa Scene Graph"
    property int priority: 90

    property var sofaScene: SofaApplication.sofaScene
    property real rowHeight: 16

    enabled: sofaScene ? sofaScene.ready : false

    ScrollView {
        anchors.fill: parent
        implicitHeight: Math.min(listView.implicitHeight, listView.contentHeight)

        ListView {
            id: listView
            implicitHeight: 400
            clip: true

            model: SofaSceneListModel {
                id: listModel
                sofaScene: root.sofaScene

                property bool dirty: true
            }

            Connections {
                target: root.sofaScene
                onStatusChanged: {
                    listView.currentIndex = -1;
                }
                onStepEnd: {
                    if(root.sofaScene.play)
                        listModel.dirty = true;
                    else if(listModel)
                        listModel.update();
                }
                onReseted: {
                    if(listModel)
                        listModel.update();
                }
                onSelectedComponentChanged: {
                    listView.currentIndex = listModel.getComponentId(sofaScene.selectedComponent);
                }
            }

            Timer {
                running: root.sofaScene.play ? true : false
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
                sofaScene.selectedComponent = listModel.getComponentById(listView.currentIndex);
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
                visible: !(SofaSceneListModel.Hidden & visibility)
                opacity: !(SofaSceneListModel.Disabled & visibility) ? 1.0 : 0.5

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    Item {
                        Layout.preferredHeight: rowHeight
                        Layout.preferredWidth: Layout.preferredHeight

                        Image {
                            anchors.fill: parent
                            visible: collapsible
                            source: (SofaSceneListModel.Disabled & visibility) ? "qrc:/icon/disabled.png" : (!(SofaSceneListModel.Collapsed & visibility) ? "qrc:/icon/downArrow.png" : "qrc:/icon/rightArrow.png")

                            MouseArea {
                                anchors.fill: parent
                                enabled: collapsible && !(SofaSceneListModel.Disabled & visibility)
                                onClicked: listView.model.setCollapsed(index, !(SofaSceneListModel.Collapsed & visibility))
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: rowHeight
                        spacing: 4

                        RowLayout {
                            Layout.fillHeight: true
                            spacing: 0

                            Rectangle {
                                id: colorIcon

                                Layout.preferredWidth: rowHeight * 0.5
                                Layout.preferredHeight: Layout.preferredWidth

                                radius: isNode ? Layout.preferredWidth * 0.5 : 0

                                color: isNode ? "black" : Qt.lighter("#FF" + Qt.md5(type).slice(4, 10), 1.5)
                                border.width: 1
                                border.color: "black"
                            }
                            Rectangle {
                                visible: multiparent
                                Layout.preferredWidth: rowHeight * 0.5
                                Layout.preferredHeight: Layout.preferredWidth

                                radius: isNode ? Layout.preferredWidth * 0.5 : 0

                                color: colorIcon.color
                                border.width: 1
                                border.color: "black"
                            }
                        }

                        Text {
                            text: (multiparent ? "Multi" : "") + (0 !== type.length || 0 !== name.length ? type + " - " + name : "")
                            color: Qt.darker(Qt.rgba((depth * 6) % 9 / 8.0, depth % 9 / 8.0, (depth * 3) % 9 / 8.0, 1.0), 1.5)
                            font.bold: isNode

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Menu {
                                id: nodeMenu

                                property QtObject sofaData: null
                                property bool nodeActivated: true

                                MenuItem {
                                    text: {
                                        nodeMenu.nodeActivated ? "Desactivate node" : "Activate node"
                                    }
                                    onTriggered: {
                                        nodeMenu.nodeActivated ? nodeMenu.sofaData.setValue(false) : nodeMenu.sofaData.setValue(true);
                                        nodeMenu.nodeActivated = nodeMenu.sofaData.value();
                                        listView.model.setCollapsed(index, visibility); // update the collapse property
                                    }
                                }

                                MenuSeparator {}

                                MenuItem {
                                    text: {
                                        "Delete node"
                                    }
                                    onTriggered: {
                                        if(listView.currentIndex === index)
                                            listView.currentIndex = -1;

                                        sofaScene.removeComponent(listModel.getComponentById(index));
                                        listModel.update();
                                    }
                                }
                            }

                            Menu {
                                id: objectMenu

                                property QtObject sofaData: null

                                MenuItem {
                                    text: {
                                        "Delete object"
                                    }
                                    onTriggered: {
                                        if(listView.currentIndex === index)
                                            listView.currentIndex = -1;

                                        sofaScene.removeComponent(listModel.getComponentById(index));
                                        listModel.update();
                                    }
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: {
                                    if(mouse.button === Qt.LeftButton) {
                                        listView.currentIndex = index;
                                    } else if(mouse.button === Qt.RightButton) {
                                        var component = listModel.getComponentById(index);

                                        if(isNode) {
                                            nodeMenu.sofaData = component.getComponentData("activated");
                                            nodeMenu.nodeActivated = nodeMenu.sofaData.value();
                                            nodeMenu.popup();
                                        } else {
                                            objectMenu.popup();
                                        }
                                    }
                                }

                                onDoubleClicked: {
                                    if(mouse.button === Qt.LeftButton)
                                        sofaDataListViewWindowComponent.createObject(SofaApplication, {"sofaScene": root.sofaScene, "sofaComponent": listModel.getComponentById(index)});
                                }

                                Component {
                                    id: sofaDataListViewWindowComponent

                                    SofaDataListViewWindow {
                                        sofaScene: root.sofaScene
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

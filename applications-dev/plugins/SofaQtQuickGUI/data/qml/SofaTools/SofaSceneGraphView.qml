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
import SofaSceneListModel 1.0

CollapsibleGroupBox {
    id: root
    implicitWidth: 0
    //implicitHeight: listView.contentHeight + 10

    title: "Sofa Scene Graph"
    property int priority: 90

    property var sofaScene: null
    property bool activatedData: true
    property real rowHeight: 16

    enabled: sofaScene ? sofaScene.ready : false

    ListView {
        id: listView
        anchors.fill: parent
        //clip: true

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

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Item {
                    Layout.preferredHeight: rowHeight
                    Layout.preferredWidth: Layout.preferredHeight

                    Image {
                        anchors.fill: parent
                        visible: collapsible
                        source: !(SofaSceneListModel.Collapsed & visibility) ? "qrc:/icon/downArrow.png" : "qrc:/icon/rightArrow.png"

                        MouseArea {
                            anchors.fill: parent
                            enabled: collapsible
                            onClicked: listView.model.setCollapsed(index, !(SofaSceneListModel.Collapsed & visibility))
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
                                activatedData ? d.sofaData.setValue(0) : d.sofaData.setValue(1)
                                activatedData = d.sofaData.value()
                                // Hide/Show children of desactivated/activated node
                                listView.model.setCollapsed(index, !(SofaSceneListModel.Collapsed & visibility))
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
                                d.sofaData = sofaScene.selectedComponent.getComponentData("activated")
                                activatedData = d.sofaData.value()
                                nodeMenu.popup()
                            }
                        }
                    }
                }
            }
        }
    }

    QtObject {
        id : d

        property QtObject sofaData
    }
}

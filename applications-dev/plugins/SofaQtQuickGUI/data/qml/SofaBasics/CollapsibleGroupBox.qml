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

import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import QtQml.Models 2.1

FocusScope {
    id: root

    property string title
    property bool collapsed: false
    property int contentMargin: 5

    /*! \internal */
    default property alias __content: container.data
    readonly property alias contentItem: container

    implicitWidth: 0
    implicitHeight: header.height + (collapsed || 0 === container.children.length ? 0 : container.implicitHeight)

    property var subWindow: null
    onCollapsedChanged: {
        var newCollapsed = collapsed;
        if(!newCollapsed && subWindow)
            closeDialog();

        collapsed = newCollapsed;
    }

    function openDialog() {
        if(subWindow)
            return;

        d.wasCollapsed = collapsed;
        collapsed = true;
        subWindow = windowComponent.createObject(root);
        subWindow.closing.connect(function(close) {
            subWindowButton.checked = false;
        });

        movableContainer.parent = subWindow.containerParent;
        subWindow.showNormal();
    }

    function closeDialog() {
        if(!root.subWindow)
            return;

        movableContainer.parent = containerParent;
        subWindow.close();
        subWindow = null;
        collapsed = d.wasCollapsed;
    }

    Layout.minimumWidth: implicitWidth
    Layout.minimumHeight: implicitHeight

    Accessible.role: Accessible.Grouping
    Accessible.name: title

    activeFocusOnTab: false

    data: [
        QtObject {
            id: d

            property bool wasCollapsed: false
        },
        Item {
            focus: true
            width: parent.width
            height: parent.height

            ColumnLayout {
                id: containerParent
                anchors.fill: parent
//                anchors.top: parent.top
//                anchors.bottom: stretchItem.top
//                anchors.left: parent.left
//                anchors.right: parent.right
                spacing: 0

                Button {
                    id: header
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                    text: root.title
                    style: ButtonStyle {
                        background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 25
                            //border.width: control.hovered ? 1 : 0
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: control.pressed ? "#ccc" : "#bbb" }
                                GradientStop { position: 0.5; color: control.pressed ? "#eee" : (control.hovered ? "#fff" : "#ddd") }
                                GradientStop { position: 1.0; color: control.pressed ? "#ccc" : "#bbb" }
                            }
                        }
                    }

                    onClicked: root.collapsed = !root.collapsed

                    ToolButton {
                        id: subWindowButton
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        anchors.margins: 2
                        anchors.rightMargin: 20
                        width: 16
                        height: 16

                        checkable: true
                        iconSource: "qrc:/icon/subWindow.png"

                        onCheckedChanged: {
                            if(checked) {
                                root.openDialog();
                            } else {
                                root.closeDialog();
                            }
                        }
                    }
                }

                ScrollView {
                    id: movableContainer
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Flickable {
                        anchors.fill: parent
                        contentHeight: container.implicitHeight

                        Item {
                            id: container
                            //anchors.fill: parent
                            implicitWidth: parent.width
                            implicitHeight: calcHeight()
                            visible: !root.collapsed || containerParent !== parent

                            property Item layoutItem: container.children.length === 1 ? container.children[0] : null
                            function calcWidth () { return container.anchors.leftMargin + container.anchors.rightMargin + (layoutItem ? (layoutItem.implicitWidth) +
                                                                         (layoutItem.anchors.fill ? layoutItem.anchors.leftMargin +
                                                                                                    layoutItem.anchors.rightMargin : 0) : container.childrenRect.width) }
                            function calcHeight () { return container.anchors.topMargin + container.anchors.bottomMargin + (layoutItem ? (layoutItem.implicitHeight) +
                                                                          (layoutItem.anchors.fill ? layoutItem.anchors.topMargin +
                                                                                                     layoutItem.anchors.bottomMargin : 0) : container.childrenRect.height) }
                        }
                    }
                }
            }

//            Rectangle {
//                id: stretchItem
//                anchors.left: parent.left
//                anchors.right: parent.right
//                y: parent.height - height
//                height: 5
//                color: stretchMouseArea.drag.active ? "red" : "darkgrey"

//                MouseArea {
//                    id: stretchMouseArea
//                    anchors.fill: parent
//                    drag.target: parent
//                    drag.axis: Drag.YAxis
//                    drag.minimumY: header.height
//                    drag.maximumY: container.implicitHeight + header.height

//                    property var test: drag.maximumY
//                    onTestChanged: console.log(test);

//                    property bool dragging: drag.active

//                    onDraggingChanged: {
//                        root.implicitHeight = Qt.binding(function() {return stretchItem.y + stretchItem.height;});
//                    }
//                }
//            }
        }]

    Component {
        id: windowComponent

        Window {
            width: 400
            height: 600
            modality: Qt.NonModal
            flags: Qt.Dialog | Qt.WindowStaysOnTopHint

            property var containerParent: windowContainerParent

            ColumnLayout {
                id: windowContainerParent
                anchors.fill: parent
            }
        }
    }
}

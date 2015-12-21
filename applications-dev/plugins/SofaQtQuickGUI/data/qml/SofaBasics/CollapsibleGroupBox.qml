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

FocusScope {
    id: root

    property string title
    property bool collapsed: false
    property int contentMargin: 5

    /*! \internal */
    default property alias __content: container.data
    readonly property alias contentItem: container

    implicitWidth: 0
    implicitHeight: !anchors.fill ? header.height + (collapsed || 0 === container.children.length ? 0 : container.calcHeight()) : 0

    Layout.minimumWidth: implicitWidth
    Layout.minimumHeight: implicitHeight

    Accessible.role: Accessible.Grouping
    Accessible.name: title

    activeFocusOnTab: false

    data: [
        Item {
            focus: true
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
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
                }

                Item {
                    visible: !root.collapsed && 0 !== container.children.length
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item {
                        id: container
                        anchors.fill: parent
                        anchors.topMargin: root.contentMargin
                        anchors.leftMargin: root.contentMargin
                        anchors.rightMargin: root.contentMargin
                        anchors.bottomMargin: root.contentMargin

                        property Item layoutItem: container.children.length === 1 ? container.children[0] : null
                        function calcWidth () { return container.anchors.leftMargin + container.anchors.rightMargin + (layoutItem ? (layoutItem.implicitWidth) +
                                                                     (layoutItem.anchors.fill ? layoutItem.anchors.leftMargin +
                                                                                                layoutItem.anchors.rightMargin : 0) : container.childrenRect.width) }
                        function calcHeight () { return container.anchors.topMargin + container.anchors.bottomMargin + (layoutItem ? (layoutItem.implicitHeight) +
                                                                      (layoutItem.anchors.fill ? layoutItem.anchors.topMargin +
                                                                                                 layoutItem.anchors.bottomMargin : 0) : container.childrenRect.height) }
                    }

//                    Rectangle {
//                        color: "black"
//                        anchors.left: parent.left
//                        anchors.right: parent.right
//                        height: 5

//                        MouseArea {
//                            anchors.fill: parent
//                            drag.target: parent
//                            drag.axis: Drag.YAxis
//                            drag.minimumY: parent.height
//                        }
//                    }
                }
            }
        }]
}

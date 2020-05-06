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
import QtQuick.Controls 2.4
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.0

RowLayout {
    id: root
    spacing: 0

    property var validator: null

    property alias beginValue: rangeBeginTextField.value
    property alias endValue: rangeEndTextField.value

    property real minimumValue: 0.0
    property real maximumValue: 1.0

    property string title: ""

    TextField {
        id: rangeBeginTextField
        Layout.preferredWidth: 34
        implicitHeight: 20
        validator: root.validator
        text: value.toString()

        property real value: root.minimumValue
        onValueChanged: {
            text = value.toString();
            cursorPosition = 0;

            if(value > rangeEndTextField.value)
                rangeEndTextField.value = value;

            rangeBegin.refresh();
        }

        onAccepted: value = Number(text);
    }

    Rectangle {
        id: rangeContainer
        Layout.fillWidth: true
        Layout.minimumWidth: implicitHeight * 2.0
        implicitHeight: 14
        radius: height * 0.5
        border.width: 1
        border.color: "#999999"

        gradient: Gradient {
            GradientStop { position: 0 ; color: "#AAAAAA" }
            GradientStop { position: 0.4 ; color: "#BBBBBB" }
            GradientStop { position: 0.6 ; color: "#BBBBBB" }
            GradientStop { position: 1 ; color: "#AAAAAA" }
        }

        onWidthChanged: {
            rangeBegin.refresh();
            rangeEnd.refresh();
        }

        Rectangle {
            id: rangeMiddle
            anchors.left: rangeBegin.right
            anchors.right: rangeEnd.left
            anchors.verticalCenter: parent.verticalCenter
            height: Math.min(width, parent.height)
            radius: width
            clip: true

            gradient: Gradient {
                GradientStop { position: 0 ; color: "#83AADE" }
                GradientStop { position: 1 ; color: "#5791DE" }
            }

            Label {
                anchors.centerIn: parent
                text: root.title
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: 8
                opacity: Math.max(0.0, Math.min(parent.width / implicitWidth - 1.0, 1.0))
            }
        }

        Rectangle {
            id: rangeBegin
            implicitWidth: parent.implicitHeight
            implicitHeight: implicitWidth
            radius: height * 0.5
            border.width: 1.0
            border.color: rangeBeginMouseArea.drag.active ? "#ccc" : "#ddd"

            onXChanged: {
                if(!rangeBeginMouseArea.drag.active)
                    return;

                var value = (rangeBegin.x) / (rangeContainer.width - rangeBegin.width - rangeEnd.width);
                value = Math.max(root.minimumValue, Math.min(root.minimumValue + value * (root.maximumValue - root.minimumValue), root.maximumValue));
                rangeBeginTextField.value = value;
            }

            function refresh() {
                var rangeWidth = rangeContainer.width - rangeBegin.width - rangeEnd.width;
                var pos = (rangeBeginTextField.value - root.minimumValue) / (root.maximumValue - root.minimumValue);
                x = Math.max(0, Math.min(pos * rangeWidth, rangeContainer.width - width));
            }

            gradient: Gradient {
                GradientStop { position: 0 ; color: rangeBeginMouseArea.drag.active ? "#ccc" : "#eee" }
                GradientStop { position: 1 ; color: rangeBeginMouseArea.drag.active ? "#bbb" : "#ddd" }
            }

            MouseArea {
                id: rangeBeginMouseArea
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: 0
                drag.maximumX: rangeEnd.x - rangeBegin.width
                drag.threshold: 0.0
            }
        }

        Rectangle {
            id: rangeEnd
            implicitWidth: parent.implicitHeight
            implicitHeight: implicitWidth
            x: parent.width - width
            radius: height * 0.5
            border.width: 1.0
            border.color: rangeEndMouseArea.drag.active ? "#ccc" : "#ddd"

            onXChanged: {
                if(!rangeEndMouseArea.drag.active)
                    return;

                var value = (rangeEnd.x - rangeBegin.width) / (rangeContainer.width - rangeBegin.width - rangeEnd.width);
                value = Math.max(root.minimumValue, Math.min(root.minimumValue + value * (root.maximumValue - root.minimumValue), root.maximumValue));
                rangeEndTextField.value = value;
            }

            function refresh() {
                var rangeWidth = rangeContainer.width - rangeBegin.width - rangeEnd.width;
                var pos = 1.0 - (rangeEndTextField.value - root.minimumValue) / (root.maximumValue - root.minimumValue);
                x = Math.max(0, Math.min(rangeContainer.width - rangeEnd.width - pos * rangeWidth, rangeContainer.width - width));
            }

            gradient: Gradient {
                GradientStop { position: 0 ; color: rangeEndMouseArea.drag.active ? "#ccc" : "#eee" }
                GradientStop { position: 1 ; color: rangeEndMouseArea.drag.active ? "#bbb" : "#ddd" }
            }

            MouseArea {
                id: rangeEndMouseArea
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: rangeBegin.x + rangeBegin.width
                drag.maximumX: rangeContainer.width - rangeEnd.width
                drag.threshold: 0.0
            }
        }
    }

    TextField {
        id: rangeEndTextField
        Layout.preferredWidth: 34
        implicitHeight: 20
        validator: root.validator
        text: value.toString()

        property real value: root.maximumValue
        onValueChanged: {
            text = value.toString();
            cursorPosition = 0;

            if(value < rangeBeginTextField.value)
                rangeBeginTextField.value = value;

            rangeEnd.refresh();
        }

        onAccepted: value = Number(text);
    }
}

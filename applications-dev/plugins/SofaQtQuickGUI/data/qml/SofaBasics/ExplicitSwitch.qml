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
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.1

Switch {
    id: root

    style: SwitchStyle {
        groove: Item {
            implicitWidth: 60
            implicitHeight: 20

            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                implicitWidth: parent.width * 0.5
                implicitHeight: parent.height
                radius: 5
                color: root.enabled ? "#468bb7" : Qt.darker("lightgrey", 1.25)
                Text {
                    color: Qt.darker("white", root.enabled ? 1.0 : 1.25)
                    anchors.centerIn: parent
                    text: "ON"
                }
            }
            Rectangle {
                anchors.right: parent.right
                implicitWidth: parent.width * 0.5
                implicitHeight: parent.height
                radius: 5
                color: Qt.darker("#C3C3C3", root.enabled ? 1.0 : 1.25)
                Text {
                    color: Qt.darker("white", root.enabled ? 1.0 : 1.25)
                    anchors.centerIn: parent
                    text: "OFF"
                }
            }
        }

        handle: Rectangle {
            implicitWidth: 30
            implicitHeight: 20
            radius: 5
            border.width: 1
            border.color: Qt.darker("grey", root.enabled ? 1.0 : 1.1)
            gradient: Gradient {
                GradientStop {position: 0.0; color: Qt.darker("white", root.enabled ? 1.0 : 1.1)}
                GradientStop {position: 1.0; color: Qt.darker("lightgrey", root.enabled ? 1.0 : 1.1)}
            }
        }
    }
}

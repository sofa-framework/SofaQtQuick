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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import SofaApplication 1.0
import SofaBasics 1.0
import SofaColorScheme 1.0

ToolBar {
    id: root
    height: 20
    anchors.margins: 0
    anchors.leftMargin: 0

    property var    sofaScene: SofaApplication.sofaScene
    property string statusMessage: ""
    property int    statusDuration: 5000

    Component.onCompleted: SofaApplication.statusBar = root

    onStatusMessageChanged: clearStatusTimer.restart()

    Timer {
        id: clearStatusTimer
        running: false
        repeat: false
        interval: statusDuration
        onTriggered: statusMessage = ""
    }

    Connections {
        target: root.sofaScene
        onStatusMessageChanged: statusMessage = root.sofaScene.statusMessage;
    }

    RowLayout {
        anchors.fill: parent

        Rectangle {
//            Layout.fillHeight: true
            Layout.preferredWidth: fpsItem.implicitWidth + 10
            color: "transparent"
            border.width: 1
            border.color: "grey"
            radius: 2

            FPSItem {
                id: fpsItem
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        Rectangle {
            width: 1
            height: 16
            border.color: "grey"
            color: "transparent"
        }

        Rectangle {
            Layout.preferredWidth: sofaTimeItem.implicitWidth + 10
            color: "transparent"
            border.width: 1
            border.color: "grey"
            radius: 2

            SofaTimeItem {
                id: sofaTimeItem
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "black"
            }
        }
        Rectangle {
            width: 1
            height: 16
            border.color: "grey"
            color: "transparent"
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumWidth: 256
            color: "transparent"
            border.width: 1
            border.color: "grey"
            radius: 2

            Label {
                id: statusLabel
                anchors.fill: parent
                anchors.margins: 5
                verticalAlignment: Text.AlignVCenter
                text: root.statusMessage
                color: "black"
            }
        }
    }
    background: Rectangle {
        color: hovered ? "#757575" : "#686868"
        border.color: "black"

        GBRect {
            anchors.top: parent.top
            anchors.topMargin: 1
            implicitHeight: parent.implicitHeight - 1
            implicitWidth: parent.implicitWidth + 2
            borderWidth: 1
            borderGradient: Gradient {
                GradientStop { position: 0.0; color: "#7a7a7a" }
                GradientStop { position: 1.0; color: "#5c5c5c" }
            }
            color: "transparent"
        }
    }
}

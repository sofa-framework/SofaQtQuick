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
import QtQuick.Layouts 1.0
import SofaApplication 1.0

ToolBar {
    id: root
    height: 20
    anchors.margins: 0
    anchors.leftMargin: 0

    property var    scene: null
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
        target: root.scene
        onStatusMessageChanged: statusMessage = root.scene.statusMessage;
    }

    RowLayout {
        anchors.fill: parent

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: fpsDisplay.implicitWidth + 10
            color: "transparent"
            border.width: 1
            border.color: "grey"
            radius: 2

            FPSDisplay {
                id: fpsDisplay
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
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
            }
        }
    }
}

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

import QtQuick 2.3
import QtQuick.Controls 1.0
import SofaApplication 1.0

Label {
    id: root

    property var sofaScene: SofaApplication.sofaScene

    text: "Time: " + timer.sceneTime.toString()

    Timer {
        id: timer
        running: sofaScene && sofaScene.ready
        repeat: true
        interval: 1
        triggeredOnStart: true
        onTriggered: sceneTime = Number(sofaScene.dataValue("@.time")).toFixed(2);

        property real sceneTime: 0.0

        readonly property alias sofaScene: root.sofaScene
        onSofaSceneChanged: sceneTime = 0.0;
    }
}

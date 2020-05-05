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

Button {    
    hoverEnabled: true
    property string iconSource: "qrc:/icon/next.png"
    property bool useHoverOpacity: true
    Image {
        id: image
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.left: parent.left
        height: parent.height
        width: parent.height
        source: iconSource
        fillMode: Image.PreserveAspectFit
        Loader {
            id: hoverOpacityLoader
            sourceComponent: useHoverOpacity ? rect : null
        }
        Component {
            id: rect
            Rectangle {
                anchors.fill: parent
                opacity: pressed ? 0.2 : 0.8
                color: pressed ? "white" : "lightgrey"
                visible: pressed || hovered
            }
        }

    }
    background: Rectangle {
        color: "transparent"
    }
}

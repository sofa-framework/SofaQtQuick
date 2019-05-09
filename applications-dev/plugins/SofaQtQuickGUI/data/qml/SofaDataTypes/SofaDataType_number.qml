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
import SofaBasics 1.0

TextField {
    id: root
    enabled: !dataObject.readOnly
    selectByMouse: true

    property var dataObject: null
    property int decimals: dataObject.properties["decimals"]

    Component.onCompleted: download();
    onAccepted:
    {
        focus = false
    }

    onActiveFocusChanged:
    {
        if(!activeFocus)
        {
            upload()
        }
    }

    function getValue()
    {
        return Number(Number(root.text).toFixed(decimals))
    }

    function incrementValue(initialValue, incrVal)
    {
        var step = dataObject.properties["step"]
        var newValue = initialValue + incrVal*step*0.1
        root.text = Number(Number(newValue).toFixed(3)).toString()
        upload()
    }

    MouseArea
    {
        anchors.fill: root
        preventStealing: true
        property bool isDragging: false
        property var initialPosition : Qt.vector2d(0,0)
        property var initialValue: 0.0

        onPressed:
        {
            isDragging = false
            initialValue = root.getValue()
            initialPosition = Qt.vector2d(mouseX, mouseY)
        }

        onPositionChanged:
        {
            var currentPosition = Qt.vector2d(mouseX, mouseY)
            if( Math.abs(currentPosition.x - initialPosition.x) > 10 )
            {
                isDragging = true
                root.incrementValue(initialValue, currentPosition.x - initialPosition.x)
            }
        }

        onReleased:
        {
            if(isDragging)
            {
                isDragging = false
            }
            else
            {
                root.forceActiveFocus(Qt.MouseFocusReason)
                root.cursorPosition = root.positionAt(mouseX, mouseY)
            }
        }
    }

    property var intValidator: IntValidator {
        Component.onCompleted: {
            var min = dataObject.properties["min"];
            if(undefined !== min)
                bottom = min;
        }
    }

    property var doubleValidator: DoubleValidator {
        decimals: root.decimals
    }

    validator: decimals > 0 ? doubleValidator : intValidator

    function download() {
        root.text = Number(Number(dataObject.value).toFixed(decimals)).toString();
        cursorPosition = 0;
        console.log("DOWNLOADING DATA FROM: "+dataObject.name)
    }

    function upload() {
        console.log("XUPLOADING DATA TO: "+dataObject.name)
        var newValue = Number(Number(root.text).toFixed(decimals));
        dataObject.value = newValue;
    }
}

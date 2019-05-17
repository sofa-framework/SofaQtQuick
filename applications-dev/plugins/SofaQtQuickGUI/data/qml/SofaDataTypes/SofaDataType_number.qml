/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - initial version from Anatoscope
    - damien.marchal@univ-lille.fr
********************************************************************/

import QtQuick 2.0
import QtQuick.Controls 2.4
import SofaBasics 1.0
import Sofa.Core.SofaData 1.0

TextField
{
    id: self
    enabled: !sofaData.readOnly
    //readOnly: sofaData.readOnly
    selectByMouse: true

    property SofaData sofaData: null
    property var properties: sofaData.properties
    readonly property int decimals: properties["decimals"]

    property int refreshCounter : 0
    onRefreshCounterChanged:
    {
        self.text = Number(Number(sofaData.value).toFixed(decimals)).toString();
    }

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

    text: Number(Number(sofaData.value).toFixed(decimals)).toString();

    function getValue()
    {
        return Number(Number(self.text).toFixed(decimals))
    }

    function incrementValue(initialValue, incrVal)
    {
        var step = self.properties["step"]
        var newValue = initialValue + incrVal*step*0.1
        self.text = Number(Number(newValue).toFixed(3)).toString()
        sofaData.value = newValue
    }

    MouseArea
    {
        anchors.fill: self
        preventStealing: true
        property bool isDragging: false
        property var initialPosition : Qt.vector2d(0,0)
        property var initialValue: 0.0

        onPressed:
        {
            isDragging = false
            initialValue = self.getValue()
            initialPosition = Qt.vector2d(mouseX, mouseY)
        }

        onPositionChanged:
        {
            var currentPosition = Qt.vector2d(mouseX, mouseY)
            if( Math.abs(currentPosition.x - initialPosition.x) > 10 )
            {
                isDragging = true
                self.incrementValue(initialValue, currentPosition.x - initialPosition.x)
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
                self.forceActiveFocus(Qt.MouseFocusReason)
                self.cursorPosition = self.positionAt(mouseX, mouseY)
            }
        }
    }

    property var intValidator: IntValidator {
        Component.onCompleted: {
            var min = self.properties.min;
            if(undefined !== min)
                bottom = min;
        }
    }

    property var doubleValidator: DoubleValidator {
        decimals: self.decimals
    }

    validator: decimals > 0 ? doubleValidator : intValidator

//    function download() {
//        root.text = Number(Number(dataObject.value).toFixed(decimals)).toString();
//        cursorPosition = 0;
//        console.log("DOWNLOADING DATA FROM: "+dataObject.name)
//    }

//    function upload() {
//        console.log("XUPLOADING DATA TO: "+dataObject.name)
//        var newValue = Number(Number(root.text).toFixed(decimals));
//        dataObject.value = newValue;
//    }
}

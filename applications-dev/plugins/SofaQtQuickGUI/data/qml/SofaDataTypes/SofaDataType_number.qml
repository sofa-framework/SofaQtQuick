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

    property var dataObject: null

    enabled: !dataObject.readOnly
    property int decimals: dataObject.properties["decimals"]

    selectByMouse: true

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

    onPressAndHold:
    {
        console.log("HELLO WORLD...")
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
        //var oldValue = Number(Number(dataObject.value).toFixed(decimals));
        //if(oldValue !== newValue)
        console.log("UPLOADING DATA TO: "+dataObject.name)
        var newValue = Number(Number(root.text).toFixed(decimals));
        dataObject.value = newValue;
    }
}

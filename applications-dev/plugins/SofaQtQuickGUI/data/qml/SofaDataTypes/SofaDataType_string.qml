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
import QtQuick.Layouts 1.3
import SofaBasics 1.0

/***************************************************************************************************
  *
  * A widget dedicated to edit Data<string> object.
  *
  *************************************************************************************************/
TextField {
    id: root

    property var sofaData: null
    readOnly: sofaData.properties.type !== "string" ? true : sofaData.properties.readOnly
    implicitWidth: parent.width
    Layout.fillWidth: true
    selectByMouse: true

    text: sofaData.value.toString()
    Connections
    {
        target: sofaData
        onValueChanged: {
            text=sofaData.value.toString()
            console.log("VALUE HAS CHANGED... "+ text)
        }
    }

    onAccepted:
    {
        console.log("VALUE HAS CHANGED... SETTING" +  text)
        sofaData.value = text
        root.focus = false
    }
    onEditingFinished:
    {
        console.log("VALUE HAS CHANGED... SETTING2" + text)
        sofaData.value = text
        root.focus = false
    }
}

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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import SofaBasics 1.0
import Sofa.Core.SofaData 1.0

/***************************************************************************************************
  *
  * A widget dedicated to visualize ComponentState
  *
  *************************************************************************************************/
RowLayout {
    property SofaData sofaData: null

    function getValue() {
        return sofaData.value + "(" + sofaData.getCounter() + ")"
    }
    function getIconFromStatus()
    {
        if(sofaData.value === "Undefined")
            return "qrc:/icon/state_bubble_1.png"
        if(sofaData.value === "Loading")
            return "qrc:/icon/state_bubble_3.png"
        if(sofaData.value === "Busy")
            return "qrc:/icon/state_bubble_3.png"
        if(sofaData.value === "Valid")
            return "qrc:/icon/state_bubble_4.png"
        if(sofaData.value === "Ready")
            return "qrc:/icon/state_bubble_4.png"
        if(sofaData.value === "Invalid")
            return "qrc:/icon/state_bubble_5.png"

        return "qrc:/icon/state_bubble_1.png"
    }

    onSofaDataChanged: {
        root.text = getValue()
    }

    Connections
    {
        target: sofaData
        onValueChanged: {
            root.text = getValue()
        }
    }
    Image {
        id: componentState
        scale: 0.8
        source: getIconFromStatus()
        opacity: 0.75
    }
    Text {
        id: root
        enabled:true
        text: getValue()
    }
}

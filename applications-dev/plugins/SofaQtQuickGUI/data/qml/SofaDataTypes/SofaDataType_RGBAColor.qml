/*
Copyright 2017, CNRS

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

Contributors:
    - damien.marchal@univ-lille1.fr
*/

import QtQuick 2.2
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.0
import SofaBasics 1.0

/***************************************************************************************************
  *
  * A widget dedicated to edit Data<RGBAColor>
  *
  *************************************************************************************************/
Row {
    id: root
    spacing : -1
    width: parent.width

    property var dataObject: null
    property var values: undefined !== dataObject.value ? dataObject.value[0] : []

    TextField {
        id: textField
        enabled: true
        width: root.width - colorChooser.width - root.spacing
        text: undefined !== dataObject.value ? dataObject.value.toString() : ""

        onAccepted: {
            dataObject.value = [textField.text.split(',').map(Number)] ;
            dataObject.upload();
            colorChooser.setValueFromArray(values) ;
        }
    }

    ColorChooser {
        id: colorChooser
        Layout.alignment: Qt.AlignTop
        width: 16
        height: 16
        color : Qt.rgba(root.values[0], root.values[1], root.values[2], root.values[3])

        onColorChanged: {
            textField.text = ""+((1.0*r)).toFixed(2)
                            +", "+((1.0*g)).toFixed(2)
                            +", "+((1.0*b)).toFixed(2)
                            +", "+((1.0*a)).toFixed(2);
            dataObject.value = [[r,g,b,a]] ;
            dataObject.upload();
        }
    }
}

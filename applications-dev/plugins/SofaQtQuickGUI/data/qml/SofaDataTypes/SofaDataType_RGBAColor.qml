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
RowLayout {
    id: root
    Layout.fillWidth: true
    spacing : -1

    property var sofaData: null
    property var values: sofaData.value[0]
    onValuesChanged: {
        colorChooser.setValueFromArray(values) ;
        _R.value = values[0]
        _G.value = values[1]
        _B.value = values[2]
        _A.value = values[3]
        sofaData.value = [values];
    }

    SpinBox {
        id: _R
        readOnly: sofaData.isReadOnly
        Layout.fillWidth: true
        value: sofaData.values[0]
        onValueChanged: {
            if (values[0] === value)
                return;
            var v = values
            v[0] = value
            values = v;
        }
        to: 1.0
        from: 0.0
        step: 0.01
        decimals: 2
        position: cornerPositions['Left']
    }
    SpinBox {
        id: _G
        readOnly: sofaData.isReadOnly
        Layout.fillWidth: true
        value: sofaData.values[1]
        onValueChanged: {
            if (values[1] === value)
                return;
            var v = values
            v[1] = value
            values = v;
        }
        to: 1.0
        from: 0.0
        step: 0.01
        decimals: 2
        position: cornerPositions['Middle']
    }
    SpinBox {
        id: _B
        readOnly: sofaData.isReadOnly
        Layout.fillWidth: true
        value: sofaData.values[2]
        onValueChanged: {
            if (values[2] === value)
                return;
            var v = values
            v[2] = value
            values = v;
        }
        to: 1.0
        from: 0.0
        step: 0.01
        decimals: 2
        position: cornerPositions['Middle']
    }
    SpinBox {
        id: _A
        readOnly: sofaData.isReadOnly
        Layout.fillWidth: true
        value: sofaData.values[3]
        onValueChanged: {
            if (values[3] === value)
                return;
            var v = values
            v[3] = value
            values = v;
        }
        to: 1.0
        from: 0.0
        step: 0.01
        decimals: 2
        position: cornerPositions['Right']
    }

    ColorChooser {
        id: colorChooser
        Layout.maximumWidth: 20
        Layout.alignment: Qt.AlignTop
        color : Qt.rgba(values[0], values[1], values[2], values[3])

        onColorChanged: {
            if (r === values[0] && g === values[1] &&
                    b === values[2] && a === values[3])
                return;
            var val = [((1.0*r)).toFixed(2), ((1.0*g)).toFixed(2), ((1.0*b)).toFixed(2), ((1.0*a)).toFixed(2)]
            root.values = val
        }
    }
}

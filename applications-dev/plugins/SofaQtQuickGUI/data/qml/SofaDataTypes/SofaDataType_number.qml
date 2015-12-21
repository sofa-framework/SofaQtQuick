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

SpinBox {
    id: root

    property var dataObject

    enabled: !dataObject.readOnly
    value: dataObject.value
    minimumValue: undefined !== dataObject.properties.min ? dataObject.properties.min : -Number.MAX_VALUE
    maximumValue: undefined !== dataObject.properties.max ? dataObject.properties.max :  Number.MAX_VALUE
    stepSize: undefined !== dataObject.properties.step ? dataObject.properties.step : 1
    decimals: undefined !== dataObject.properties.decimals ? dataObject.properties.decimals : 0

    Binding {
        target: dataObject
        property: "value"
        value: root.value
        when: !dataObject.readOnly
    }
}


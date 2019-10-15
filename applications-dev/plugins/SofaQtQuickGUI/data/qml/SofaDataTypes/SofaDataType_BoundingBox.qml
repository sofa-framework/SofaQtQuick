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
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4
import SofaBasics 1.0
import SofaColorScheme 1.0
import Sofa.Core.SofaData 1.0

ColumnLayout {
    id: root
    spacing: -1

    height: 40
    implicitHeight: height
    property SofaData dataObject: null


    property var values: []
    onDataObjectChanged: {
        var valStrings = dataObject.value.split(' ')
        for (var i = 0 ; i < valStrings.length ; i++) {
            values.push(Number(valStrings[i]))
        }
    }
    RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: -1

        Text {
            id: minLabel
            Layout.preferredWidth: Math.max(minLabel.implicitWidth, maxLabel.implicitWidth) - 5
            text: "Min "
        }

        SpinBox {
            id: spinBox0
            property int index: 0

            Layout.fillWidth: true

            showIndicators: false
            enabled: !dataObject.isReadOnly

            value: Number(root.values[index])
            onValueChanged: {
                if(!dataObject.isReadOnly) {
                    root.values[index] = value;
                    dataObject.value = root.values.join(' ');
                }
            }

            Binding {
                target: spinBox0
                property: "value"
                value: root.values[spinBox0.index]
            }

            position: cornerPositions["TopLeft"]
        }

        SpinBox {
            id: spinBox1
            property int index: 1

            Layout.fillWidth: true

            showIndicators: false
            enabled: !dataObject.isReadOnly

            value: Number(root.values[index])
            onValueChanged: {
                if(!dataObject.isReadOnly) {
                    root.values[index] = value;
                    dataObject.value = root.values.join(' ');
                }
            }

            Binding {
                target: spinBox1
                property: "value"
                value: root.values[spinBox1.index]
            }
            position: cornerPositions["Middle"]
        }

        SpinBox {
            id: spinBox2
            property int index: 2

            Layout.fillWidth: true

            showIndicators: false
            enabled: !dataObject.isReadOnly

            value: Number(root.values[index])
            onValueChanged: {
                if(!dataObject.isReadOnly) {
                    root.values[index] = value;
                    dataObject.value = root.values.join(' ');
                }
            }

            Binding {
                target: spinBox2
                property: "value"
                value: root.values[spinBox2.index]
            }
            position: cornerPositions["TopRight"]
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: -1

        Text {
            id: maxLabel
            text: "Max "
            Layout.preferredWidth: Math.max(minLabel.implicitWidth, maxLabel.implicitWidth) - 5
        }

        SpinBox {
            id: spinBox3
            property int index: 3

            Layout.fillWidth: true

            showIndicators: false
            enabled: !dataObject.isReadOnly

            value: Number(root.values[index])
            onValueChanged: {
                if(!dataObject.isReadOnly) {
                    root.values[index] = value;
                    dataObject.value = root.values.join(' ');
                }
            }

            Binding {
                target: spinBox3
                property: "value"
                value: root.values[spinBox3.index]
            }
            position: cornerPositions["BottomLeft"]

        }

        SpinBox {
            id: spinBox4
            property int index: 4

            Layout.fillWidth: true

            showIndicators: false
            enabled: !dataObject.isReadOnly

            value: Number(root.values[index])
            onValueChanged: {
                if(!dataObject.isReadOnly) {
                    root.values[index] = value;
                    dataObject.value = root.values.join(' ');
                }
            }

            Binding {
                target: spinBox4
                property: "value"
                value: root.values[spinBox4.index]
            }
            position: cornerPositions["Middle"]
        }

        SpinBox {
            id: spinBox5
            property int index: 5

            Layout.fillWidth: true

            showIndicators: false
            enabled: !dataObject.isReadOnly

            value: Number(root.values[index])
            onValueChanged: {
                if(!dataObject.isReadOnly) {
                    root.values[index] = value;
                    dataObject.value = root.values.join(' ');
                }
            }

            Binding {
                target: spinBox5
                property: "value"
                value: root.values[spinBox5.index]
            }
            position: cornerPositions["BottomRight"]
        }
    }
}

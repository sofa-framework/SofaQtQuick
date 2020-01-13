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

RowLayout {
    id: root
    spacing: -1
    property SofaData sofaData: null
    implicitWidth: parent.implicitWidth
    property var values: []
    onSofaDataChanged: {
        var valStrings = sofaData.value.split(' ')
        for (var i = 0 ; i < valStrings.length ; i++) {
            values.push(Number(valStrings[i]))
        }
        spinBox0.value = values[0]
        spinBox1.value = values[1]
        spinBox2.value = values[2]
        spinBox3.value = values[3]
        spinBox4.value = values[4]
        spinBox5.value = values[5]
    }


    ColumnLayout {
        spacing: -1
        Layout.fillWidth: true
        Label {
            Layout.preferredWidth: 30
            color: "black"
            text: "Min"
        }
        Label {
            Layout.preferredWidth: 30
            color: "black"
            text: "Max"
        }
    }
    ColumnLayout {
        spacing: -1
        Layout.fillWidth: true
        SpinBox {
            id: spinBox0
            property int index: 0

            Layout.fillWidth: true
            showIndicators: false
            readOnly: sofaData.properties.readOnly

            value: root.values[index]
            onValueChanged: {
                if(!sofaData.properties.readOnly) {
                    root.values[index] = value;
                    sofaData.value = root.values.join(' ');
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
            id: spinBox3
            property int index: 3

            Layout.fillWidth: true

            showIndicators: false
            readOnly: sofaData.properties.readOnly

            value: root.values[index]
            onValueChanged: {
                if(!sofaData.properties.readOnly) {
                    root.values[index] = value;
                    sofaData.value = root.values.join(' ');
                }
            }

            Binding {
                target: spinBox3
                property: "value"
                value: root.values[spinBox3.index]
            }

            position: cornerPositions["BottomLeft"]
        }
    }



    ColumnLayout {
        spacing: -1
        Layout.fillWidth: true
        SpinBox {
            id: spinBox1
            property int index: 1

            Layout.fillWidth: true
            showIndicators: false
            readOnly: sofaData.properties.readOnly

            value: root.values[index]
            onValueChanged: {
                if(!sofaData.properties.readOnly) {
                    root.values[index] = value;
                    sofaData.value = root.values.join(' ');
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
            id: spinBox4
            property int index: 4

            Layout.fillWidth: true

            showIndicators: false
            readOnly: sofaData.properties.readOnly

            value: root.values[index]
            onValueChanged: {
                if(!sofaData.properties.readOnly) {
                    root.values[index] = value;
                    sofaData.value = root.values.join(' ');
                }
            }

            Binding {
                target: spinBox4
                property: "value"
                value: root.values[spinBox4.index]
            }

            position: cornerPositions["Middle"]
        }
    }


    ColumnLayout {
        spacing: -1
        Layout.fillWidth: true
        SpinBox {
            id: spinBox2
            property int index: 2

            Layout.fillWidth: true

            showIndicators: false
            readOnly: sofaData.properties.readOnly

            value: root.values[index]
            onValueChanged: {
                if(!sofaData.properties.readOnly) {
                    root.values[index] = value;
                    sofaData.value = root.values.join(' ');
                }
            }

            Binding {
                target: spinBox2
                property: "value"
                value: root.values[spinBox2.index]
            }

            position: cornerPositions["TopRight"]
        }
        SpinBox {
            id: spinBox5
            property int index: 5

            Layout.fillWidth: true

            showIndicators: false
            readOnly: sofaData.properties.readOnly

            value: root.values[index]
            onValueChanged: {
                if(!sofaData.properties.readOnly) {
                    root.values[index] = value;
                    sofaData.value = root.values.join(' ');
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

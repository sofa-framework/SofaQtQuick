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
import QtQuick.Layouts 1.1
import SofaBasics 1.0
import SofaData 1.0
import ImagePlaneModel 1.0
import ImagePlaneView 1.0

GridLayout {
    id: root
    columns: 2

    property var dataObject: null

    ImagePlaneModel {
        id: imagePlane

        sofaData: root.dataObject.sofaData
    }

    Component {
        id: sliceComponent

        ColumnLayout {
            readonly property int sliceIndex: slice.index

            Rectangle {
                id: rectangle
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: slice.implicitWidth
                Layout.preferredHeight: slice.implicitHeight
                color: "black"

                border.color: "darkgrey"
                border.width: 1

                ImagePlaneView {
                    id: slice
                    anchors.fill: parent
                    anchors.margins: rectangle.border.width

                    imagePlaneModel: imagePlane
                    index: slider.value
                    axis: sliceAxis

                    Component.onCompleted: update();

                    Connections {
                        target: sofaScene
                        onStepEnd: slice.update()
                    }
                }
            }

            Slider {
                id: slider
                Layout.fillWidth: true

                minimumValue: 0
                maximumValue: slice.length > 0 ? slice.length - 1 : 0
                value: slice.length / 2
                stepSize: 1
                tickmarksEnabled: true
            }
        }
    }

    Loader {
        id: planeX
        Layout.fillWidth: true
        Layout.fillHeight: true

        sourceComponent: sliceComponent
        property int sliceAxis: 0
        readonly property int sliceIndex: item ? item.sliceIndex : 0
    }

    Loader {
        id: planeY
        Layout.fillWidth: true
        Layout.fillHeight: true

        sourceComponent: sliceComponent
        property int sliceAxis: 1
        readonly property int sliceIndex: item ? item.sliceIndex : 0
    }

    Item {
        id: info
        Layout.fillWidth: true
        Layout.fillHeight: true

        TextArea {
            anchors.fill: parent
            readOnly: true

            text: "Info:\n\n" +
                  "x: " + planeX.sliceIndex + "\n" +
                  "y: " + planeY.sliceIndex + "\n" +
                  "z: " + planeZ.sliceIndex + "\n"
        }
    }

    Loader {
        id: planeZ
        Layout.fillWidth: true
        Layout.fillHeight: true

        sourceComponent: sliceComponent
        property int sliceAxis: 2
        readonly property int sliceIndex: item ? item.sliceIndex : 0
    }
}


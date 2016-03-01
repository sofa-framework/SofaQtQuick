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
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaData 1.0
import ImagePlaneModel 1.0
import ImagePlaneView 1.0

GridLayout {
    id: root
    columns: 2

    property var dataObject: null

    ImagePlaneModel {
        id: model

        sofaData: root.dataObject.sofaData
    }

    Component {
        id: sliceComponent

        ColumnLayout {
            readonly property int sliceIndex: imagePlaneView.index

            Rectangle {
                id: rectangle
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: imagePlaneView.implicitWidth
                Layout.preferredHeight: imagePlaneView.implicitHeight
                color: "black"

                border.color: "darkgrey"
                border.width: 1

                ImagePlaneView {
                    id: imagePlaneView
                    anchors.fill: parent
                    anchors.margins: rectangle.border.width

                    imagePlaneModel: model
                    index: slider.value
                    axis: sliceAxis

                    Component.onCompleted: update();

                    Connections {
                        target: sofaScene
                        onStepEnd: imagePlaneView.update()
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    Layout.preferredWidth: Layout.preferredHeight
                    Layout.preferredHeight: 18
                    Layout.alignment: Qt.AlignCenter
                    iconSource: "qrc:/icon/subWindow.png"
                    visible: showSubWindow

                    onClicked: windowComponent.createObject(SofaApplication, {"sliceComponent": sliceComponent, "sliceAxis": sliceAxis});
                }

                Slider {
                    id: slider
                    Layout.fillWidth: true

                    minimumValue: 0
                    maximumValue: imagePlaneView.length > 0 ? imagePlaneView.length - 1 : 0
                    stepSize: 1
                    tickmarksEnabled: true

                    value: model.currentIndex(imagePlaneView.axis);
                    onValueChanged: model.setCurrentIndex(imagePlaneView.axis, value);
                }
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
        property bool showSubWindow: true
    }

    Loader {
        id: planeY
        Layout.fillWidth: true
        Layout.fillHeight: true

        sourceComponent: sliceComponent
        property int sliceAxis: 1
        readonly property int sliceIndex: item ? item.sliceIndex : 0
        property bool showSubWindow: true
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
        property bool showSubWindow: true
    }

    Component {
        id: windowComponent

        Window {
            id: window
            width: 600
            height: 600
            modality: Qt.NonModal
            flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
            visible: true
            color: "lightgrey"

//            Component.onCompleted: {
//                width = Math.max(width, loader.implicitWidth);
//                height = Math.min(height, loader.implicitHeight);
//            }

            property var sliceComponent: null
            property alias sliceAxis: loader.sliceAxis

            title: "Plane " + String.fromCharCode('X'.charCodeAt(0) + sliceAxis)

            ColumnLayout {
                anchors.fill: parent

                Loader {
                    id: loader
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onImplicitHeightChanged: window.height = Math.max(window.height, loader.implicitHeight);

                    sourceComponent: window.sliceComponent
                    property int sliceAxis: -1
                    property bool showSubWindow: false
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}


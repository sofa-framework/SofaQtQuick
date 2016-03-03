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
import QtQml.Models 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaData 1.0
import ImagePlaneModel 1.0
import ImagePlaneView 1.0

ColumnLayout {
    id: root

    property var dataObject: null

    ImagePlaneModel {
        id: planeModel

        sofaData: root.dataObject.sofaData
    }

    GroupBox {
        id: showGroup
        Layout.fillWidth: true

        title: "Show"

        property int count2D: (showPlaneX2D.checked ? 1 : 0) + (showPlaneY2D.checked ? 1 : 0) + (showPlaneZ2D.checked ? 1 : 0)

        GridLayout {
            anchors.fill: parent
            Layout.alignment: Qt.AlignCenter
            columns: 4

            Label {
                text: "2D"
            }

            CheckBox {
                id: showPlaneX2D
                text: "X"
                checked: true
            }
            CheckBox {
                id: showPlaneY2D
                text: "Y"
                checked: true
            }
            CheckBox {
                id: showPlaneZ2D
                text: "Z"
                checked: true
            }

            Label {
                text: "3D"
            }

            CheckBox {
                id: showPlaneX3D
                text: "X"
                checked: true
            }
            CheckBox {
                id: showPlaneY3D
                text: "Y"
                checked: true
            }
            CheckBox {
                id: showPlaneZ3D
                text: "Z"
                checked: true
            }
        }
    }

//    RowLayout {
//        Layout.fillWidth: true

//        Loader {
//            id: planeX
//            visible: showPlaneX2D.checked

//            Layout.fillWidth: true

//            sourceComponent: sliceComponent
//            property int sliceAxis: 0
//            readonly property int sliceIndex: item ? item.sliceIndex : 0
//            property bool showSubWindow: true
//        }

//        Loader {
//            id: planeY
//            visible: showPlaneY2D.checked

//            Layout.fillWidth: true

//            sourceComponent: sliceComponent
//            property int sliceAxis: 1
//            readonly property int sliceIndex: item ? item.sliceIndex : 0
//            property bool showSubWindow: true
//        }

//        Loader {
//            id: planeZ
//            visible: showPlaneZ2D.checked

//            Layout.fillWidth: true

//            sourceComponent: sliceComponent
//            property int sliceAxis: 2
//            readonly property int sliceIndex: item ? item.sliceIndex : 0
//            property bool showSubWindow: true
//        }
//    }

    GridView {
        Layout.fillWidth: true
        Layout.preferredHeight: 200

        cellWidth: width * 0.5
        //cellWidth: 200
        cellHeight: cellWidth

        model: ListModel {
            id: model

            ListElement {
                sliceAxis: 0
                showSubWindow: true
            }
            ListElement {
                sliceAxis: 1
                showSubWindow: true
            }
            ListElement {
                sliceAxis: 2
                showSubWindow: true
            }
        }

        delegate: sliceComponent
    }

// components

    Component {
        id: sliceComponent

        ColumnLayout {
            width: GridView.view.cellWidth
            height: GridView.view.cellHeight
            //readonly property int sliceIndex: imagePlaneView.index

            Component.onCompleted: console.log("c", width, height)

            onWidthChanged: console.log("w", width);
            onHeightChanged: console.log("h", height);

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Qt.rgba(0 == sliceAxis, 1 == sliceAxis, 2 == sliceAxis, 1.0)
            }

//            Flickable {
//                id: flickable
//                Layout.fillWidth: true
//                Layout.fillHeight: true
//                //Layout.preferredWidth: imagePlaneView.implicitWidth
//                //Layout.preferredHeight: imagePlaneView.implicitHeight
//                clip: true

//                height: width//implicitHeight
//                //onHeightChanged: console.log("+", height)

//                //Layout.onPreferredHeightChanged: console.log(Layout.preferredHeight)

//                boundsBehavior: Flickable.StopAtBounds
//                contentWidth: rectangle.width * rectangle.scale
//                contentHeight: rectangle.height * rectangle.scale

//                rebound: Transition {}

//                Rectangle {
//                    id: rectangle
////                    implicitWidth: flickable.width
////                    implicitHeight: flickable.height
//                    width: 100
//                    height: 100
//                    transformOrigin: Item.TopLeft
//                    color: "black"

//                    //onHeightChanged: console.log("-", height)

//                    border.color: "darkgrey"
//                    border.width: 1

//                    ImagePlaneView {
//                        id: imagePlaneView
//                        anchors.fill: parent
//                        anchors.margins: rectangle.border.width

//                        imagePlaneModel: planeModel
//                        index: slider.value
//                        axis: sliceAxis

//                        Component.onCompleted: update();

//                        Connections {
//                            target: sofaScene
//                            onStepEnd: imagePlaneView.update()
//                        }
//                    }
//                }
//            }

//            MouseArea {
//                anchors.fill: flickable
//                acceptedButtons: Qt.NoButton

//                onWheel: {
//                    if(0 === wheel.angleDelta.y)
//                        return;

//                    var inPosition = mapToItem(rectangle, wheel.x, wheel.y);
//                    if(!rectangle.contains(inPosition))
//                        return;

//                    var zoomSpeed = 1.0;

//                    var boundary = 2.0;
//                    var zoom = Math.max(-boundary, Math.min(wheel.angleDelta.y / 120.0, boundary)) / boundary;
//                    if(zoom < 0.0) {
//                        zoom = 1.0 + 0.5 * zoom;
//                        zoom /= zoomSpeed;
//                    }
//                    else {
//                        zoom = 1.0 + zoom;
//                        zoom *= zoomSpeed;
//                    }

//                    rectangle.scale = Math.max(1.0, rectangle.scale * zoom);

//                    var outPosition = mapFromItem(rectangle, inPosition.x, inPosition.y);

//                    flickable.contentX += (outPosition.x - wheel.x);
//                    flickable.contentY += (outPosition.y - wheel.y);
//                    flickable.returnToBounds();
//                }
//            }

//            RowLayout {
//                Layout.fillWidth: true
//                spacing: 0

//                Button {
//                    Layout.preferredWidth: Layout.preferredHeight
//                    Layout.preferredHeight: 18
//                    Layout.alignment: Qt.AlignCenter
//                    iconSource: "qrc:/icon/subWindow.png"
//                    visible: showSubWindow

//                    onClicked: windowComponent.createObject(SofaApplication, {"sliceComponent": sliceComponent, "sliceAxis": sliceAxis});
//                }

//                Slider {
//                    id: slider
//                    Layout.fillWidth: true

//                    minimumValue: 0
//                    maximumValue: imagePlaneView.length > 0 ? imagePlaneView.length - 1 : 0
//                    stepSize: 1
//                    tickmarksEnabled: true

//                    value: planeModel.currentIndex(imagePlaneView.axis);
//                    onValueChanged: planeModel.setCurrentIndex(imagePlaneView.axis, value);
//                }

//                Label {
//                    horizontalAlignment: Qt.AlignRight
//                    text: (imagePlaneView.length - 1).toString() + "/" + (imagePlaneView.length - 1).toString()

//                    Component.onCompleted: {
//                        Layout.preferredWidth = implicitWidth; // set value to avoid binding
//                        text = Qt.binding(function() {return slider.value.toString() + "/" + (imagePlaneView.length - 1).toString();});
//                    }
//                }
//            }
        }
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


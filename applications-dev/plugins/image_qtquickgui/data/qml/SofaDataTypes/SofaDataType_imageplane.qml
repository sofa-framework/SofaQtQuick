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

    property var controller: null
    onDataObjectChanged: {
        if(dataObject) {
            var sofaComponent = dataObject.sofaData.sofaComponent();
            var sofaScene = sofaComponent.sofaScene();
            controller = sofaScene.retrievePythonScriptController(sofaComponent, "ImagePlaneController")
            console.log("controller", controller);
        }
    }

    property bool allPlanesLoaded: (Loader.Ready === planeX.status) && (Loader.Ready === planeY.status) && (Loader.Ready === planeZ.status)
    onAllPlanesLoadedChanged: if(allPlanesLoaded) requestRefresh();

    signal requestRefresh()

    ImagePlaneModel {
        id: model

        sofaData: root.dataObject.sofaData
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

                Flickable {
                    id: flickable
                    anchors.fill: parent
                    anchors.margins: parent.border.width
                    clip: true

                    boundsBehavior: Flickable.StopAtBounds
                    contentWidth: flickableContent.width * flickableContent.scale
                    contentHeight: flickableContent.height * flickableContent.scale

                    rebound: Transition {}

                    Item {
                        id: flickableContent
                        width: imagePlaneView.implicitWidth
                        height: imagePlaneView.implicitHeight
                        transformOrigin: Item.TopLeft

                        scale: maxScale
                        property real maxScale: Math.min(flickable.width / imagePlaneView.implicitWidth, flickable.height / imagePlaneView.implicitHeight);

                        ImagePlaneView {
                            id: imagePlaneView
                            anchors.fill: parent

                            imagePlaneModel: model
                            index: slider.value
                            axis: sliceAxis

                            Component.onCompleted: update();

                            Connections {
                                target: sofaScene
                                onStepEnd: imagePlaneView.update()
                            }

                            Item {
                                id: pointCanvas
                                anchors.centerIn: parent
                                width: imagePlaneView.paintedWidth
                                height: imagePlaneView.paintedHeight

                                property alias imagePlaneView: imagePlaneView

                                property var points: Object()

                                Connections {
                                    target: root
                                    onRequestRefresh: pointCanvas.updatePoints();
                                }

                                imagePlaneView.onIndexChanged: updatePoints();
                                imagePlaneView.onAxisChanged: updatePoints();

                                function addPoint(x, y) {
                                    if(!root.controller)
                                        return;

                                    var sofaComponent = dataObject.sofaData.sofaComponent();
                                    var sofaScene = sofaComponent.sofaScene();

                                    var worldPosition = model.toWorldPoint(imagePlaneView.axis, imagePlaneView.index, Qt.point(x, y));
                                    console.log(x, y, ";", worldPosition);
                                    var id = sofaScene.sofaPythonInteractor.call(controller, "addPoint", worldPosition.x, worldPosition.y, worldPosition.z);

                                    root.requestRefresh();

                                    return id;
                                }

                                function removePoint(id) {
                                    if(!root.controller)
                                        return;

                                    var sofaComponent = dataObject.sofaData.sofaComponent();
                                    var sofaScene = sofaComponent.sofaScene();
                                    sofaScene.sofaPythonInteractor.call(controller, "removePoint", id);

                                    root.requestRefresh();
                                }

                                function removePointAt(x, y, brushSize) {
                                    if(!root.controller)
                                        return;

                                    if(undefined === brushSize)
                                        brushSize = 1.0;

                                    var brushRadius = brushSize * 0.5;

                                    var sofaComponent = dataObject.sofaData.sofaComponent();
                                    var sofaScene = sofaComponent.sofaScene();

                                    for(var id in points) {
                                        if(!points.hasOwnProperty(id))
                                            continue;

                                        var point = points[id];
                                        var distance = Qt.vector2d(x - point.x, y - point.y).length();
                                        if(distance < brushRadius)
                                            sofaScene.sofaPythonInteractor.call(controller, "removePoint", id);
                                    }

                                    root.requestRefresh();
                                }

                                function updatePoints() {
                                    var sofaComponent = dataObject.sofaData.sofaComponent();
                                    var sofaScene = sofaComponent.sofaScene();
                                    var updatedPoints = sofaScene.sofaPythonInteractor.call(controller, "getPoints");

                                    // mark old points for deletion
                                    var pointsToRemove = Object();
                                    for(var stringId in points) {
                                        if(!points.hasOwnProperty(stringId))
                                            continue;

                                        pointsToRemove[stringId] = points[stringId];
                                    }

                                    // add / update points
                                    for(var stringId in updatedPoints) {
                                        if(!updatedPoints.hasOwnProperty(stringId))
                                            continue;

                                        var id = parseInt(stringId);

                                        if(!points.hasOwnProperty(id)) { // add new points
                                            points[id] = updatedPoints[stringId];
                                            //console.log("add", id, points[id]);
                                            pointComponent.createObject(pointCanvas, {'pointId': id});
                                        }
                                        else { // keep old point still presents
                                            points[id] = updatedPoints[stringId];
                                            delete pointsToRemove[id];
                                        }
                                    }

                                    // clear old points that has been removed from the model
                                    for(var stringId in pointsToRemove) {
                                        if(!pointsToRemove.hasOwnProperty(stringId))
                                            continue;

                                        for(var i = 0; i < children.length; ++i)
                                            if(stringId === children[i].pointId) {
                                                children[i].destroy();
                                                break;
                                            }
                                    }

                                    pointsChanged();
                                }

                                Component {
                                    id: pointComponent

                                    Item {
                                        id: item

                                        property int pointId: -1
                                        property var point: pointCanvas.points[pointId]

                                        Component.onCompleted: update();
                                        onPointChanged: update();

                                        function update() {
                                            if(null === point)
                                                return;

                                            var screenPosition = model.toImagePoint(imagePlaneView.axis, Qt.vector3d(point.x, point.y, point.z))

                                            x = screenPosition.x;
                                            y = screenPosition.y;
                                        }

                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 5 / flickableContent.scale
                                            height: width
                                            radius: width * 0.5
                                            color: "red"

                                            Component.onCompleted: {
                                                console.log("creating point", pointId, pointCanvas.points[pointId], "at:", x, y);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                MouseArea {
                    anchors.fill: flickable
                    acceptedButtons: Qt.AllButtons

                    onClicked: {
                        var position = mapToItem(pointCanvas, mouse.x + 0.5, mouse.y + 0.5);
                        if(!pointCanvas.contains(position))
                            return;

                        if(Qt.LeftButton === mouse.button)
                            pointCanvas.addPoint(position.x, position.y);
                        else if(Qt.RightButton === mouse.button)
                            pointCanvas.removePointAt(position.x, position.y, 5);
                    }

                    onWheel: {
                        if(0 === wheel.angleDelta.y)
                            return;

                        var inPosition = mapToItem(flickableContent, wheel.x, wheel.y);
                        if(!flickableContent.contains(inPosition)) {
                            return;

                            // TODO: project
                            inPosition.x = Math.max(0.0, Math.min(inPosition.x, flickableContent.width - 1.0));
                            inPosition.y = Math.max(0.0, Math.min(inPosition.y, flickableContent.height - 1.0));
                        }

                        var zoomSpeed = 1.0;

                        var boundary = 2.0;
                        var zoom = Math.max(-boundary, Math.min(wheel.angleDelta.y / 120.0, boundary)) / boundary;
                        if(zoom < 0.0) {
                            zoom = 1.0 + 0.5 * zoom;
                            zoom /= zoomSpeed;
                        }
                        else {
                            zoom = 1.0 + zoom;
                            zoom *= zoomSpeed;
                        }

                        flickableContent.scale = Math.max(flickableContent.maxScale, flickableContent.scale * zoom);

                        var outPosition = mapFromItem(flickableContent, inPosition.x, inPosition.y);

                        flickable.contentX += (outPosition.x - wheel.x);
                        flickable.contentY += (outPosition.y - wheel.y);
                        flickable.returnToBounds();
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 0

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

                Label {
                    horizontalAlignment: Qt.AlignRight
                    text: (imagePlaneView.length - 1).toString() + "/" + (imagePlaneView.length - 1).toString()

                    Component.onCompleted: {
                        Layout.preferredWidth = implicitWidth; // set value to avoid binding
                        text = Qt.binding(function() {return slider.value.toString() + "/" + (imagePlaneView.length - 1).toString();});
                    }
                }
            }
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


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

import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaData 1.0
import ImagePlaneModel 1.0
import ImagePlaneView 1.0

Item {
    id: root
    implicitWidth: layout.implicitWidth
    implicitHeight: layout.implicitHeight

    property var imagePlaneModel: null
    property var sofaComponent: null
    property var controller: null
    property bool showModels: true

    onSofaComponentChanged: {
        if(sofaComponent) {
            var sofaScene = sofaComponent.sofaScene();
            controller = sofaScene.retrievePythonScriptController(sofaComponent, "ImagePlaneController", "SofaImage.Tools");
        }
    }

    readonly property int planeIndex: imagePlaneView.index
    property int planeAxis: 0

    signal requestRefresh()
    onRequestRefresh: refresh();

    function refresh() {
        pointCanvas.updatePoints();
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        spacing: 0

        RangeSlider {
            id: intensityRangeSlider
            Layout.fillWidth: true

            title: "Intensity"

            minimumValue: 0.0
            maximumValue: 1.0
        }

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

                contentWidth: flickableContent.width * flickableContent.scale
                contentHeight: flickableContent.height * flickableContent.scale
                boundsBehavior: Flickable.StopAtBounds

                onWidthChanged: returnToBounds();
                onHeightChanged: returnToBounds();

                Item {
                    id: flickableContent
                    width: Math.max(flickable.width / scale, imagePlaneView.rotatedImplicitWidth)
                    height: Math.max(flickable.height / scale, imagePlaneView.rotatedImplicitHeight)
                    transformOrigin: Item.TopLeft

                    Component.onCompleted: scale = minScale;

                    property real minScale: Math.min(flickable.width / imagePlaneView.rotatedImplicitWidth, flickable.height / imagePlaneView.rotatedImplicitHeight);
                    property real maxScale: minScale * Math.max(imagePlaneView.rotatedImplicitWidth, imagePlaneView.rotatedImplicitHeight);

                    ImagePlaneView {
                        id: imagePlaneView
                        anchors.centerIn: parent
                        smooth: false

                        readonly property real rotatedImplicitWidth: Math.abs(implicitWidth * Math.cos(rotation * Math.PI / 180.0) - implicitHeight * Math.sin(rotation * Math.PI / 180.0))
                        readonly property real rotatedImplicitHeight: Math.abs(implicitWidth * Math.sin(rotation * Math.PI / 180.0) + implicitHeight * Math.cos(rotation * Math.PI / 180.0))

                        imagePlaneModel: root.imagePlaneModel
                        index: slider.value
                        axis: root.planeAxis
                        showModels: root.showModels

                        minIntensity: intensityRangeSlider.beginValue
                        maxIntensity: intensityRangeSlider.endValue

                        Component.onCompleted: update();

                        Connections {
                            target: root.sofaComponent ? root.sofaComponent.sofaScene() : null
                            onStepEnd: imagePlaneView.update()
                        }

                        Item {
                            id: pointCanvas
                            anchors.fill: parent

                            property var points: Object()

                            property alias index: imagePlaneView.index
                            property alias axis: imagePlaneView.axis
                            onIndexChanged: updatePoints();
                            onAxisChanged: updatePoints();

                            function addPoint(x, y) {
                                if(!root.sofaComponent || !root.controller)
                                    return;

                                var sofaScene = root.sofaComponent.sofaScene();

                                var worldPosition = root.imagePlaneModel.toWorldPoint(axis, index, Qt.point(x, y));
                                var id = sofaScene.sofaPythonInteractor.call(root.controller, "addPoint", worldPosition.x, worldPosition.y, worldPosition.z);

                                root.requestRefresh();

                                return id;
                            }

                            function removePoint(id) {
                                var removed = false;

                                if(!root.sofaComponent || !root.controller)
                                    return removed;

                                var sofaScene = root.sofaComponent.sofaScene();
                                removed = sofaScene.sofaPythonInteractor.call(root.controller, "removePoint", id);

                                root.requestRefresh();

                                return removed;
                            }

                            function removePointAt(x, y, brushSize) {
                                var removed = false;

                                if(!root.sofaComponent || !root.controller)
                                    return removed;

                                if(undefined === brushSize)
                                    brushSize = Screen.devicePixelRatio * 5.0 / flickableContent.scale;

                                var brushRadius = brushSize * 0.5;

                                var sofaScene = root.sofaComponent.sofaScene();

                                for(var stringId in points) {
                                    if(!points.hasOwnProperty(stringId))
                                        continue;

                                    var id = parseInt(stringId);

                                    var point = points[stringId];
                                    var worldPosition = Qt.vector3d(point.position[0], point.position[1], point.position[2]);
                                    var screenPosition = root.imagePlaneModel.toPlanePoint(imagePlaneView.axis, worldPosition)

                                    var distance = Qt.vector2d(x - screenPosition.x, y - screenPosition.y).length();
                                    if(distance < brushRadius)
                                        if(sofaScene.sofaPythonInteractor.call(root.controller, "removePoint", id))
                                            removed = true;
                                }

                                root.requestRefresh();

                                return removed;
                            }

                            function updatePoints() {
                                if(!root.sofaComponent || !root.controller)
                                    return;

                                var sofaScene = root.sofaComponent.sofaScene();
                                var updatedPoints = sofaScene.sofaPythonInteractor.call(root.controller, "getPoints");

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
                                    var point = updatedPoints[stringId];

                                    var worldPosition = Qt.vector3d(point.position[0], point.position[1], point.position[2]);
                                    if(!imagePlaneView.containsPoint(worldPosition))
                                        continue;

                                    if(!points.hasOwnProperty(id)) { // add new points
                                        points[id] = point;
                                        pointComponent.createObject(pointCanvas, {'pointId': id});
                                    }
                                    else { // keep old point still presents
                                        points[id] = point;
                                        delete pointsToRemove[stringId];
                                    }
                                }

                                // clear old points that has been removed from the model
                                for(var stringId in pointsToRemove) {
                                    if(!pointsToRemove.hasOwnProperty(stringId))
                                        continue;

                                    var id = parseInt(stringId);

                                    for(var i = 0; i < children.length; ++i) {
                                        if(id === children[i].pointId) {
                                            children[i].pointId = -1;
                                            break;
                                        }
                                    }

                                    delete points[id];
                                }

                                pointsChanged();
                            }

                            Component {
                                id: pointComponent

                                Item {
                                    id: item

                                    property int pointId: -1

                                    onPointIdChanged: update();

                                    Connections {
                                        target: pointCanvas
                                        onPointsChanged: item.update();
                                    }

                                    function update() {
                                        if(-1 === pointId)
                                        {
                                            destroy();
                                            return;
                                        }

                                        var point = pointCanvas.points[pointId];

                                        var worldPosition = Qt.vector3d(point.position[0], point.position[1], point.position[2]);
                                        var screenPosition = root.imagePlaneModel.toPlanePoint(imagePlaneView.axis, worldPosition)

                                        x = screenPosition.x;
                                        y = screenPosition.y;

                                        var color = "red";
                                        if(undefined !== point.color)
                                            color = Qt.rgba(point.color[0], point.color[1], point.color[2], 1.0);

                                        dot.color = color;
                                    }

                                    Rectangle {
                                        id: dot
                                        anchors.centerIn: parent
                                        width: 5
                                        height: width
                                        radius: width * 0.5
                                        scale: 1.0 / flickableContent.scale
                                        color: "red"
                                    }
                                }
                            }
                        }
                    }
                }
            }

            MouseArea {
                anchors.fill: flickable
                acceptedButtons: Qt.LeftButton
                propagateComposedEvents: true

                onPressed: {
                    if(mouse.modifiers & Qt.ShiftModifier)
                        mouse.accepted = true;
                    else
                        mouse.accepted = false;
                }

                onClicked: {
                    var position = mapToItem(pointCanvas, mouse.x, mouse.y);
                    position = Qt.point(position.x + 0.5, position.y + 0.5);
                    if(!pointCanvas.contains(position))
                        return;

                    var brushSize = Screen.devicePixelRatio * 5.0 / flickableContent.scale;
                    if(!pointCanvas.removePointAt(position.x, position.y, brushSize))
                        pointCanvas.addPoint(position.x, position.y);
                }

                onWheel: {
                    if(0 === wheel.angleDelta.y)
                        return;

                    var inPosition = mapToItem(imagePlaneView, wheel.x, wheel.y);
                    var projectedPosition = Qt.point(wheel.x, wheel.y);
                    if(!imagePlaneView.contains(inPosition)) {
                        inPosition.x = Math.max(0.0, Math.min(inPosition.x, imagePlaneView.width - 0.0001));
                        inPosition.y = Math.max(0.0, Math.min(inPosition.y, imagePlaneView.height - 0.0001));
                        projectedPosition = mapFromItem(imagePlaneView, inPosition.x, inPosition.y);
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

                    flickableContent.scale = Math.max(flickableContent.minScale, Math.min(flickableContent.scale * zoom, flickableContent.maxScale));

                    var outPosition = mapFromItem(imagePlaneView, inPosition.x, inPosition.y);

                    flickable.contentX += (outPosition.x - projectedPosition.x);
                    flickable.contentY += (outPosition.y - projectedPosition.y);

                    //flickable.returnToBounds();
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 0

            IconButton {
                Layout.preferredWidth: Layout.preferredHeight
                Layout.preferredHeight: implicitHeight + 10
                iconSource: "qrc:/icon/rotateClockwise.png"
                onClicked: imagePlaneView.rotation += 90.0;
            }

            Slider {
                id: slider
                Layout.fillWidth: true

                minimumValue: 0
                maximumValue: imagePlaneView.length > 0 ? imagePlaneView.length - 1 : 0
                stepSize: 1
                tickmarksEnabled: true

                value: root.imagePlaneModel.currentIndex(imagePlaneView.axis)
                onValueChanged: root.imagePlaneModel.setCurrentIndex(imagePlaneView.axis, value);
            }

            TextField {
                Layout.preferredWidth: lengthLabel.implicitWidth + 3
                text: slider.value.toString()

                onAccepted: slider.value = Number(text)
            }

            Label {
                id: lengthLabel
                Layout.preferredWidth: implicitWidth
                horizontalAlignment: Qt.AlignRight
                text: "/" + (imagePlaneView.length - 1).toString()
            }
        }
    }
}

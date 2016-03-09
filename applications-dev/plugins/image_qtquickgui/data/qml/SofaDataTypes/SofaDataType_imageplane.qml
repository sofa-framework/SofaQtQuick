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

    ColumnLayout {
        id: info
        Layout.fillWidth: true
        Layout.fillHeight: true

        TextArea {
            id: infoTextArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            readOnly: true

            Connections {
                target: root
                onRequestRefresh: {
                    var points = sofaScene.sofaPythonInteractor.call(root.controller, "getPoints");
                    var text = "";

                    for(var stringId in points) {
                        if(!points.hasOwnProperty(stringId))
                            continue;

                        var point = points[stringId];
                        var worldPosition = Qt.vector3d(point.position.x, point.position.y, point.position.z);
                        var imagePosition = model.toImagePoint(worldPosition);

                        text += "P" + stringId + ": (" + Math.round(imagePosition.x).toFixed(0) + ", " + Math.round(imagePosition.y).toFixed(0) + ", " + Math.round(imagePosition.z).toFixed(0) + ") -> (" + worldPosition.x.toFixed(3) + ", " + worldPosition.y.toFixed(3) + ", " + worldPosition.z.toFixed(3) + ")\n";
                    }

                    if(0 === text.length)
                        infoTextArea.text = "Shift + Left click to add / remove points, put a ImagePlaneController in the same context than the sofa component that is owning this data to control how points are added / removed";
                    else
                        infoTextArea.text = text;
                }
            }
        }

        Button {
            Layout.fillWidth: true
            text: "Clear points"
            onClicked: {
                sofaScene.sofaPythonInteractor.call(root.controller, "clearPoints");
                root.requestRefresh();
            }
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

                    contentWidth: flickableContent.width * flickableContent.scale
                    contentHeight: flickableContent.height * flickableContent.scale
                    boundsBehavior: Flickable.StopAtBounds

                    Item {
                        id: flickableContent
                        width: Math.max(flickable.width / scale, imagePlaneView.implicitWidth)
                        height: Math.max(flickable.height / scale, imagePlaneView.implicitHeight)
                        transformOrigin: Item.TopLeft

                        scale: minScale
                        property real minScale: Math.min(flickable.width / imagePlaneView.implicitWidth, flickable.height / imagePlaneView.implicitHeight);
                        property real maxScale: minScale * Math.max(imagePlaneView.implicitWidth, imagePlaneView.implicitHeight);

                        ImagePlaneView {
                            id: imagePlaneView
                            anchors.centerIn: parent

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
                                anchors.fill: parent

                                property var points: Object()

                                Connections {
                                    target: root
                                    onRequestRefresh: pointCanvas.updatePoints();
                                }

                                property alias index: imagePlaneView.index
                                property alias axis: imagePlaneView.axis
                                onIndexChanged: updatePoints();
                                onAxisChanged: updatePoints();

                                function addPoint(x, y) {
                                    if(!root.controller)
                                        return;

                                    var sofaComponent = dataObject.sofaData.sofaComponent();
                                    var sofaScene = sofaComponent.sofaScene();

                                    var worldPosition = model.toWorldPoint(axis, index, Qt.point(x, y));
                                    var id = sofaScene.sofaPythonInteractor.call(root.controller, "addPoint", worldPosition.x, worldPosition.y, worldPosition.z);

                                    root.requestRefresh();

                                    return id;
                                }

                                function removePoint(id) {
                                    var removed = false;

                                    if(!root.controller)
                                        return removed;

                                    var sofaComponent = dataObject.sofaData.sofaComponent();
                                    var sofaScene = sofaComponent.sofaScene();
                                    removed = sofaScene.sofaPythonInteractor.call(root.controller, "removePoint", id);

                                    root.requestRefresh();

                                    return removed;
                                }

                                function removePointAt(x, y, brushSize) {
                                    var removed = false;

                                    if(!root.controller)
                                        return removed;

                                    if(undefined === brushSize)
                                        brushSize = Screen.devicePixelRatio * 5.0 / flickableContent.scale;

                                    var brushRadius = brushSize * 0.5;

                                    var sofaComponent = dataObject.sofaData.sofaComponent();
                                    var sofaScene = sofaComponent.sofaScene();

                                    for(var stringId in points) {
                                        if(!points.hasOwnProperty(stringId))
                                            continue;

                                        var id = parseInt(stringId);

                                        var point = points[stringId];
                                        var worldPosition = Qt.vector3d(point.position.x, point.position.y, point.position.z);
                                        var screenPosition = model.toPlanePoint(imagePlaneView.axis, worldPosition)

                                        var distance = Qt.vector2d(x - screenPosition.x, y - screenPosition.y).length();
                                        if(distance < brushRadius)
                                            if(sofaScene.sofaPythonInteractor.call(root.controller, "removePoint", id))
                                                removed = true;
                                    }

                                    root.requestRefresh();

                                    return removed;
                                }

                                function updatePoints() {
                                    var sofaComponent = dataObject.sofaData.sofaComponent();
                                    var sofaScene = sofaComponent.sofaScene();
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

                                        console.log("point:", point);
                                        for(var key in point) {
//                                            if(!point.hasOwnProperty(key))
//                                                continue;

                                            console.log("key", key, " = ", point[key]);
                                        }

                                        var worldPosition = Qt.vector3d(point.position.x, point.position.y, point.position.z);
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

                                        function update() {
                                            if(-1 === pointId)
                                            {
                                                destroy();
                                                return;
                                            }

                                            var point = pointCanvas.points[pointId];

                                            var worldPosition = Qt.vector3d(point.position.x, point.position.y, point.position.z);
                                            var screenPosition = model.toPlanePoint(imagePlaneView.axis, worldPosition)

                                            x = screenPosition.x;
                                            y = screenPosition.y;

                                            var color = "red";
                                            if(undefined !== point.color)
                                                color = Qt.rgba(point.color.r, point.color.g, point.color.b, 1.0);

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


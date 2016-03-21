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
import SofaImage 1.0
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
            if(!dataObject || !root.controller)
                return;

            var sofaComponent = dataObject.sofaData.sofaComponent();
            if(!sofaComponent)
                return;

            var sofaScene = sofaComponent.sofaScene();
            if(!sofaScene)
                return;

            controller = sofaScene.retrievePythonScriptController(sofaComponent, "ImagePlaneController", "SofaImage.Tools");
        }

        refreshAll();
    }

    Connections {
        target: root.dataObject
        onUpdated: root.refreshAll();
    }

    property bool allPlanesLoaded: (Loader.Ready === planeX.status) && (Loader.Ready === planeY.status) && (Loader.Ready === planeZ.status)
    onAllPlanesLoadedChanged: if(allPlanesLoaded) refreshAll();

    function refreshAll() {
        if(planeX.item)
            planeX.item.refresh();

        if(planeY.item)
            planeY.item.refresh();

        if(planeZ.item)
            planeZ.item.refresh();

        infoTextArea.refresh();
    }

    ImagePlaneModel {
        id: model

        sofaData: root.dataObject.sofaData
    }

    Loader {
        id: planeX
        Layout.fillWidth: true
        Layout.fillHeight: true

        sourceComponent: planeComponent
        property int planeAxis: 0
        readonly property int planeIndex: item ? item.planeIndex : 0
        property bool showSubWindow: true

        Connections {
            target: planeX.item
            onRequestRefresh: {
                if(planeY.item)
                    planeY.item.refresh();

                if(planeZ.item)
                    planeZ.item.refresh();

                infoTextArea.refresh();
            }
        }
    }

    Loader {
        id: planeY
        Layout.fillWidth: true
        Layout.fillHeight: true

        sourceComponent: planeComponent
        property int planeAxis: 1
        readonly property int planeIndex: item ? item.planeIndex : 0
        property bool showSubWindow: true

        Connections {
            target: planeY.item
            onRequestRefresh: {
                if(planeX.item)
                    planeX.item.refresh();

                if(planeZ.item)
                    planeZ.item.refresh();

                infoTextArea.refresh();
            }
        }
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

            function refresh() {
                var text = "";

                if(dataObject && root.controller) {
                    var sofaComponent = dataObject.sofaData.sofaComponent();
                    if(sofaComponent) {
                        var sofaScene = sofaComponent.sofaScene();
                        if(sofaScene) {
                            var points = sofaScene.sofaPythonInteractor.call(root.controller, "getPoints");
                            for(var stringId in points) {
                                if(!points.hasOwnProperty(stringId))
                                    continue;

                                var point = points[stringId];
                                var worldPosition = Qt.vector3d(point.position[0], point.position[1], point.position[2]);
                                var imagePosition = model.toImagePoint(worldPosition);

                                text += "P" + stringId + ": (" + Math.round(imagePosition.x).toFixed(0) + ", " + Math.round(imagePosition.y).toFixed(0) + ", " + Math.round(imagePosition.z).toFixed(0) + ") -> (" + worldPosition.x.toFixed(3) + ", " + worldPosition.y.toFixed(3) + ", " + worldPosition.z.toFixed(3) + ")\n";
                            }
                        }
                    }
                }

                if(0 === text.length)
                    infoTextArea.text = "Shift + Left click to add / remove points, put a ImagePlaneController in the same context than the sofa component that is owning this data to control how points are added / removed";
                else
                    infoTextArea.text = text;
            }
        }

        Button {
            Layout.fillWidth: true
            text: "Clear points"
            onClicked: {
                if(!dataObject || !root.controller)
                    return;

                var sofaComponent = dataObject.sofaData.sofaComponent();
                if(!sofaComponent)
                    return;

                var sofaScene = sofaComponent.sofaScene();
                if(!sofaScene)
                    return;

                sofaScene.sofaPythonInteractor.call(root.controller, "clearPoints");
                root.refreshAll();
            }
        }
    }

    Loader {
        id: planeZ
        Layout.fillWidth: true
        Layout.fillHeight: true

        sourceComponent: planeComponent
        property int planeAxis: 2
        readonly property int planeIndex: item ? item.planeIndex : 0
        property bool showSubWindow: true

        Connections {
            target: planeZ.item
            onRequestRefresh: {
                if(planeX.item)
                    planeX.item.refresh();

                if(planeY.item)
                    planeY.item.refresh();

                infoTextArea.refresh();
            }
        }
    }

    Component {
        id: planeComponent

        SofaImagePlaneItem {
            controller: root.controller
            sofaComponent: root.dataObject && root.dataObject.sofaData ? root.dataObject.sofaData.sofaComponent() : null
            planeAxis: parent.planeAxis
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

            property var planeComponent: null
            property alias planeAxis: loader.planeAxis

            title: "Plane " + String.fromCharCode('X'.charCodeAt(0) + planeAxis)

            ColumnLayout {
                anchors.fill: parent

                Loader {
                    id: loader
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onImplicitHeightChanged: window.height = Math.max(window.height, loader.implicitHeight);

                    sourceComponent: window.planeComponent
                    property int planeAxis: -1
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


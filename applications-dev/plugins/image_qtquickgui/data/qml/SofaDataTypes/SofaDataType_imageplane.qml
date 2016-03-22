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

ColumnLayout {
    id: root

    property var dataObject: null
    property var sofaComponent: null

    property var controller: null
    onDataObjectChanged: {
        if(dataObject) {
            sofaComponent = dataObject.sofaData.sofaComponent();
            if(sofaComponent) {
                var sofaScene = sofaComponent.sofaScene();
                if(sofaScene)
                    controller = sofaScene.retrievePythonScriptController(sofaComponent, "ImagePlaneController", "SofaImage.Tools");
            }
        }

        refreshAll();
    }

    Connections {
        target: root.dataObject
        onUpdated: root.refreshAll();
    }

    readonly property alias planeX: container.planeX
    readonly property alias planeY: container.planeY
    readonly property alias planeZ: container.planeZ
    readonly property alias infoTextArea: container.infoTextArea

    Component.onCompleted: refreshAll();

    function refreshAll() {
        if(planeX)
            planeX.refresh();

        if(planeY)
            planeY.refresh();

        if(planeZ)
            planeZ.refresh();

        if(infoTextArea)
            infoTextArea.refresh();
    }

    ImagePlaneModel {
        id: model

        sofaData: root.dataObject.sofaData
    }

// maim

    GroupBox {
        Layout.fillWidth: true

        RowLayout {
            anchors.fill: parent
            anchors.topMargin: 2
            anchors.bottomMargin: anchors.topMargin
            spacing: 5

            Item {
                Layout.fillWidth: true
                Layout.minimumWidth: showLabel.implicitWidth
                implicitHeight: showLabel.implicitHeight

                Label {
                    id: showLabel
                    anchors.fill: parent
                    text: "Show"
                }
            }

            CheckBox {
                id: xCheckBox
                text: "X"

                Component.onCompleted: checked = (-1 !== model.currentIndex(axis))

                readonly property int axis: 0
                property int previousIndex: -1
                onCheckedChanged: {
                    if(!checked) {
                        previousIndex = model.currentIndex(axis);
                        model.setCurrentIndex(axis, -1);
                    } else {
                        var currentIndex = model.currentIndex(axis);
                        if(-1 !== currentIndex)
                            previousIndex = currentIndex;

                        if(-1 === previousIndex)
                            previousIndex = model.length(axis) / 2;

                        model.setCurrentIndex(axis, previousIndex);
                    }
                }
            }

            CheckBox {
                id: yCheckBox
                text: "Y"

                Component.onCompleted: checked = (-1 !== model.currentIndex(axis))

                readonly property int axis: 1
                property int previousIndex: -1
                onCheckedChanged: {
                    if(!checked) {
                        previousIndex = model.currentIndex(axis);
                        model.setCurrentIndex(axis, -1);
                    } else {
                        var currentIndex = model.currentIndex(axis);
                        if(-1 !== currentIndex)
                            previousIndex = currentIndex;

                        if(-1 === previousIndex)
                            previousIndex = model.length(axis) / 2;

                        model.setCurrentIndex(axis, previousIndex);
                    }
                }
            }

            CheckBox {
                id: zCheckBox
                text: "Z"

                Component.onCompleted: checked = (-1 !== model.currentIndex(axis))

                readonly property int axis: 2
                property int previousIndex: -1
                onCheckedChanged: {
                    if(!checked) {
                        previousIndex = model.currentIndex(axis);
                        model.setCurrentIndex(axis, -1);
                    } else {
                        var currentIndex = model.currentIndex(axis);
                        if(-1 !== currentIndex)
                            previousIndex = currentIndex;

                        if(-1 === previousIndex)
                            previousIndex = model.length(axis) / 2;

                        model.setCurrentIndex(axis, previousIndex);
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.minimumWidth: showLabel.implicitWidth
            }
        }
    }

    Item {
        id: container
        Layout.fillWidth: true
        Layout.fillHeight: true

        property var planeX: null
        property var planeY: null
        property var planeZ: null
        property var infoTextArea: null

        implicitHeight: layout ? layout.implicitHeight: 0
        property var layout: null

        property int planeNumber: root.sofaComponent ? (xCheckBox.checked ? 1 : 0) + (yCheckBox.checked ? 1 : 0) + (zCheckBox.checked ? 1 : 0) : 0
        onPlaneNumberChanged: refresh();

        Component.onCompleted: refresh();

        function refresh() {
            if(layout)
                layout.destroy();

            layout = gridLayoutComponent.createObject(container, {'anchors.fill': container});

            if(xCheckBox.checked)
                planeX = planeXComponent.createObject(layout, {'Layout.fillWidth': true})

            if(yCheckBox.checked)
                planeY = planeYComponent.createObject(layout, {'Layout.fillWidth': true})

            if(zCheckBox.checked)
                planeZ = planeZComponent.createObject(layout, {'Layout.fillWidth': true})

            infoTextArea = infoComponent.createObject(layout, {'Layout.fillWidth': true, 'Layout.columnSpan': 1 + (planeNumber + 1) % 2})
        }
    }

// component

    Component {
        id: gridLayoutComponent

        GridLayout {
            columns: 2
            columnSpacing: 0
            rowSpacing: 0
        }
    }

    Component {
        id: planeXComponent

        SofaImagePlaneItem {
            sofaComponent: root.sofaComponent
            controller: root.controller
            imagePlaneModel: model
            planeAxis: 0

            property bool showSubWindow: true

            onRequestRefresh: {
                if(root.planeY)
                    root.planeY.refresh();

                if(root.planeZ)
                    root.planeZ.refresh();

                if(root.infoTextArea)
                    root.infoTextArea.refresh();
            }
        }
    }

    Component {
        id: planeYComponent

        SofaImagePlaneItem {
            sofaComponent: root.sofaComponent
            controller: root.controller
            imagePlaneModel: model
            planeAxis: 1

            property bool showSubWindow: true

            onRequestRefresh: {
                if(root.planeX)
                    root.planeX.refresh();

                if(root.planeZ)
                    root.planeZ.refresh();

                if(root.infoTextArea)
                    root.infoTextArea.refresh();
            }
        }
    }

    Component {
        id: planeZComponent

        SofaImagePlaneItem {
            sofaComponent: root.sofaComponent
            controller: root.controller
            imagePlaneModel: model
            planeAxis: 2

            property bool showSubWindow: true

            onRequestRefresh: {
                if(root.planeX)
                    root.planeX.refresh();

                if(root.planeY)
                    root.planeY.refresh();

                if(root.infoTextArea)
                    root.infoTextArea.refresh();
            }
        }
    }

    Component {
        id: infoComponent

        ColumnLayout {
            id: info
            Layout.fillWidth: true
            Layout.fillHeight: true

            Component.onCompleted: refresh();

            function refresh() {
                var text = "";

                if(root.sofaComponent && root.controller) {
                    if(root.sofaComponent) {
                        var sofaScene = root.sofaComponent.sofaScene();
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
                    textArea.text = "Shift + Left click to add / remove points, put a ImagePlaneController in the same context than the sofa component that is owning this data to control how points are added / removed";
                else
                    textArea.text = text;
            }

            TextArea {
                id: textArea
                Layout.fillWidth: true
                Layout.fillHeight: true
                readOnly: true
            }

            Button {
                Layout.fillWidth: true
                text: "Clear points"
                onClicked: {
                    if(!root.sofaComponent || !root.controller)
                        return;

                    var sofaScene = root.sofaComponent.sofaScene();
                    if(!sofaScene)
                        return;

                    sofaScene.sofaPythonInteractor.call(root.controller, "clearPoints");
                    root.refreshAll();
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


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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import SofaBasics 1.0
import SofaApplication 1.0
import CameraView 1.0

CameraView {
    readonly property string docstring :
        "This view is rendering your scene from the camera defined in your scene. <br>
         It is not possible to edit or move the camera location in this view unless <br>
         your scene contains user interactions specific components."

    id: root

    clip: true
    mirroredHorizontally: false
    mirroredVertically: false
    antialiasingSamples: 2
    sofaScene: SofaApplication.sofaScene

    property var idComboList: ListModel {
        id: cameraNameItems
    }

    property int uiId: 0
    property int previousUiId: uiId
    onUiIdChanged: {
        SofaApplication.uiSettings.replace(previousUiId, uiId);
    }

    implicitWidth: 800
    implicitHeight: 600

    Timer
    {
        id: rescanForCameraTimer
        interval: 1000
        repeat: true
        onTriggered: {
            var cameraList = sofaScene.componentsByType("BaseCamera")

            if(cameraList.size()!==0)
            {
                idComboList.clear()
                for (var i = 0; i < cameraList.size(); ++i)
                {
                    idComboList.append({text: cameraList.at(i).getName()})
                }
                recreateCamera()
                stop()
            }
        }
    }

    Component.onCompleted: {
        SofaApplication.addSofaViewer(root);

        if(!SofaApplication.focusedSofaViewer)
            forceActiveFocus();

        if(root.sofaScene && root.sofaScene.ready)
            recreateCamera();

        rescanForCameraTimer.start()
    }

    Component.onDestruction: {
        SofaApplication.removeSofaViewer(root);
    }

    onActiveFocusChanged: {
        if(activeFocus)
            SofaApplication.setFocusedSofaViewer(root);
    }

    Connections {
        target: SofaApplication
        onFocusedSofaViewerChanged: {
            if((root === SofaApplication.focusedSofaViewer || !SofaApplication.focusedSofaViewer) && !root.activeFocus)
                root.forceActiveFocus();
            else
                focus = false;
        }
    }

    property alias busyIndicator: busyIndicator
    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        width: 100
        height: width
        running: !root.hideBusyIndicator && (root.sofaScene ? root.sofaScene.status === SofaScene.Loading : false)
    }

    // camera
    Component {
        id: cameraComponent
        SofaCamera {}
    }

    property bool defaultCameraOrthographic: false
    property bool keepCamera: false
    property bool hideBusyIndicator: false

    function updateCameraFromIndex(index)
    {
        if(camera)
        {
            var listCameraInSofa = sofaScene.componentsByType("BaseCamera");
            camera.sofaComponent = (sofaScene.componentsByType("BaseCamera").at(index));
            if(listCameraInSofa.size() === 0)
            {
                console.log("No Camera in the scene")
            }
            if(listCameraInSofa.size() < index)
            {
                console.log("Error while trying to load camera number "+ index)
            }
            else
            {
                camera.sofaComponent = listCameraInSofa.at(index);
            }
        }
    }

    function recreateCamera() {
        if(camera && !keepCamera) {
            camera.destroy();
            camera = null;
        }
        if(!camera)
        {
            camera = cameraComponent.createObject(root, {orthographic: defaultCameraOrthographic} );
            camera.bindCameraFromScene(root.SofaScene, 0);

            //Todo fetch index from somewhere
            var defaultIndex = 0;
            updateCameraFromIndex(0);

            if(!keepCamera)
                viewAll();
        }
    }

    Connections {
        target: root.sofaScene
        onStatusChanged: {
            if(root.sofaScene && SofaScene.Ready === root.sofaScene.status)
                root.recreateCamera();
        }
    }

    // mouse interaction
    property alias mouseArea: mouseArea
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        enabled: sofaScene && sofaScene.ready

        onPressed: {
            forceActiveFocus();
            root.mousePressed(mouse);
        }

        onReleased: {
            if(root.sofaScene)
                root.sofaScene.mouseReleased(mouse, root);
        }

//        onWheel: {
//            if(root.sofaScene)
//                root.sofaScene.mouseWheel(wheel, root);

//            wheel.accepted = true;
//        }

        onPositionChanged: {
            if(root.sofaScene)
                root.sofaScene.mouseMove(mouse, root);
        }
    }

    // keyboard interaction forwarded to the sofaScene.
    Keys.onPressed: {
        if(event.isAutoRepeat) {
            event.accepted = true;
            return;
        }

        if(root.sofaScene)
            root.sofaScene.keyPressed(event);

        uiArea.keyPressed(event)
        event.accepted = true;
    }

    Keys.onReleased: {
        if(event.isAutoRepeat) {
            event.accepted = true;
            return;
        }

        if(root.sofaScene)
            root.sofaScene.keyReleased(event);

        event.accepted = true;
    }


    signal mousePressed(var mouse)
    onMousePressed: {
        console.log("picking with new picker:")
        var particle = pickParticle2(camera.getOrigin(mouse.x, mouse.y), camera.getDirection(mouse.x, mouse.y));
        console.log("particleIdx: " + particle !== null ? particle.particleIndex : "none")
        console.log("picking with old picker:")
        particle = pickParticle(Qt.point(Math.floor(mouse.x), Math.floor(mouse.y)));
        console.log("particleIdx: " + particle !== null ? particle.particleIndex : "none")
    }

    // visual info

    readonly property alias crosshairGizmo: crosshairGizmo
    Item {
        id: crosshairGizmo
        anchors.centerIn: parent
        visible: false

        function show() {
            popAnimation.complete();
            visible = true;
        }

        function hide() {
            popAnimation.complete();
            visible = false;
        }

        function pop() {
            popAnimation.restart();
        }

        SequentialAnimation {
            id: popAnimation

            ScriptAction    {script: {crosshairGizmo.visible = true;}}
            NumberAnimation {target:  crosshairGizmo; properties: "opacity"; from: 1.0; to: 0.0; duration: 2000;}
            ScriptAction    {script: {crosshairGizmo.visible = false; crosshairGizmo.opacity = crosshairGizmo.defaultOpacity;}}
        }

        readonly property real defaultOpacity: 0.75
        opacity: defaultOpacity
        property color color: "red"
        property real size: Math.min(root.width, root.height) / 20.0
        property real thickness: 1

        Rectangle {
            anchors.centerIn: parent
            color: crosshairGizmo.color
            width: crosshairGizmo.size
            height: crosshairGizmo.thickness
        }

        Rectangle {
            anchors.centerIn: parent
            color: crosshairGizmo.color
            width: crosshairGizmo.thickness
            height: crosshairGizmo.size
        }
    }

    property bool highlightIfFocused: SofaApplication.sofaViewers.length > 1
    Rectangle {
        id: borderHighlighting
        anchors.fill: parent

        color: "transparent"
        border.width: 2
        border.color: "red"

        enabled: root.highlightIfFocused && root === SofaApplication.focusedSofaViewer
        onEnabledChanged: {
            if(enabled)
                visible = true;
            else
                visible = false;
        }

        onVisibleChanged: if(!visible) opacity = 0.0

        SequentialAnimation {
            running: borderHighlighting.visible

            NumberAnimation {
                target: borderHighlighting
                property: "opacity"
                to: 1.0
                duration: 200
            }
            NumberAnimation {
                target: borderHighlighting
                property: "opacity"
                from: 1.0
                to: 0.5
                duration: 800
            }
        }
    }

    QmlUIArea
    {
        id: uiArea
        canvasSources: SofaApplication.sofaScene.canvas
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
    }
}

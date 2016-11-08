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
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import SofaBasics 1.0
import SofaApplication 1.0
import SofaInteractors 1.0
import SofaSceneViewer 1.0
import SofaScene 1.0

SofaSceneViewer {
    id: root

    clip: true

    sofaScene: SofaApplication.sofaScene

    implicitWidth: 800
    implicitHeight: 600

    Component.onCompleted: {
        //if(!SofaApplication.focusedSofaViewer)
        //    forceActiveFocus();
    }

    Component.onDestruction: {
    }

    onActiveFocusChanged: {
        //if(activeFocus)
        //    SofaApplication.setFocusedSofaViewer(root);
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        width: 100
        height: width
        running: !hideBusyIndicator && (sofaScene ? sofaScene.status === SofaScene.Loading : false);
    }

    Label {
        anchors.fill: parent
        visible: sofaScene ? sofaScene.status === SofaScene.Error : false
        color: "red"
        wrapMode: Text.WordWrap
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: sofaScene ? "Error during sofa scene loading\n" + sofaScene.source.toString().replace("///", "/").replace("file:", "") : "No sofa scene object"
    }

    property bool hideBusyIndicator: false

    // screenshot / video
    function takeScreenshot(savePath) {
        if(undefined === savePath)
            savePath = "Captured/Screen/" + root.formatDateForScreenshot() + ".png";

        if(-1 === savePath.lastIndexOf("."))
            savePath += ".png";

        if(root.width.toFixed(0) == captureWidthTextField.text && root.height.toFixed(0) == captureHeightTextField.text)
            root.saveScreenshot(savePath);
        else
            root.saveScreenshotWithResolution(savePath, Number(captureWidthTextField.text), Number(captureHeightTextField.text));
    }

    function startVideoRecording(savePath) {
        videoRecordingPrivate.videoPath = savePath;
        videoRecordingPrivate.saveVideo = true;
    }

    function stopVideoRecording() {
        videoRecordingPrivate.saveVideo = false;
    }

    property var _videoRecordingPrivate: QtObject {
        id: videoRecordingPrivate

        property string videoPath: ""
        property bool saveVideo: false
        onSaveVideoChanged: {
            if(!saveVideo)
                return;

            videoFrameNumber = 0;

            saveVideoFrame();
        }

        property int videoFrameNumber: 0
        property var sceneConnections: Connections {
            target: root.sofaScene && videoRecordingPrivate.saveVideo ? root.sofaScene : null
            onStepEnd: videoRecordingPrivate.saveVideoFrame();
        }

        function saveVideoFrame() {
            var savePath = videoRecordingPrivate.videoPath;
            if(0 === savePath.length)
                savePath = "Captured/Movie/" + SofaApplication.formatDateForScreenshot() + "/movie.png";

            var dotIndex = savePath.lastIndexOf(".");
            if(-1 === dotIndex) {
                savePath += ".png"
                dotIndex = savePath.lastIndexOf(".");
            }

            savePath = savePath.slice(0, dotIndex) + "_" + (videoRecordingPrivate.videoFrameNumber++).toString() + savePath.slice(dotIndex);

            root.saveScreenshot(savePath);
        }
    }

    // mouse events are forwarded to the sofa scene.
    property alias mouseArea: mouseArea
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        enabled: sofaScene && sofaScene.ready
        //hoverEnabled: root.interactor ? root.interactor.hoverEnabled : false

        onClicked: {
            root.forceActiveFocus();
            console.log("Click on the sofa view")
            //if(root.interactor)
            //    root.interactor.mouseClicked(mouse, root);
        }

        onPressed: {
            root.forceActiveFocus();
            console.log("Forward to sofa scene")
        }

        onReleased: {
            console.log("Forward to sofa scene")
        }

        onWheel: {
            console.log("Forward to sofa scene")
            wheel.accepted = true;
        }

        onPositionChanged: {
            console.log("Forward to sofa scene")
        }
    }

    // keyboard event are forwarded to the sofa scene.
    Keys.onPressed: {
        if(event.isAutoRepeat) {
            event.accepted = true;
            return;
        }

        if(root.sofaScene)
            root.sofaScene.keyPressed(event);

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
}

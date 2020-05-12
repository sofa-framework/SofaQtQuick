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

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import SofaBasics 1.0
import SofaBaseScene 1.0
import SofaApplication 1.0
import CameraView 1.0
import SofaCameraListModel 1.0

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
    property int minimumValue: 2
    property int maximumValue: 16

    property bool configurable: true

    property int uiId: 0
    property int previousUiId: uiId
    onUiIdChanged: {
        SofaApplication.uiSettings.replace(previousUiId, uiId);
    }

    implicitWidth: 800
    implicitHeight: 600

    Component.onCompleted: {
        SofaApplication.addSofaViewer(root);

        if(!SofaApplication.focusedSofaViewer)
            forceActiveFocus();

        if(root.sofaScene && root.sofaScene.ready)
        {
            recreateCamera(0);
        }


    }

    Component.onDestruction: {
        SofaApplication.removeSofaViewer(root);
    }

    Connections {
        target: sofaScene
        onAnimateChanged: {
            if (sofaScene.animate) {
                animateBorder.running = true
                root.forceActiveFocus()
            }
        }
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

    // camera
    Component {
        id: cameraComponent
        SofaCamera {}
    }

    property var cameraListModel: SofaCameraListModel {
        onModelReset: {
            root.recreateCamera(0)
        }
        Component.onCompleted: {
            rescanForCameraTimer.start()
        }
    }

    property bool defaultCameraOrthographic: false
    property bool keepCamera: false
    property bool hideBusyIndicator: false

    function destroyCamera() {
        if (root.camera !== null) {
            root.camera.destroy();
            root.camera = null;
        }
    }

    function recreateCamera(idx) {
        destroyCamera()
        if (idx < cameraListModel.rowCount() && idx >= 0) {

            root.camera = cameraComponent.createObject(root, {orthographic: defaultCameraOrthographic} );
            root.camera.bindCameraFromScene(root.sofaScene, idx)
            root.camera.sofaComponent = root.sofaScene.componentsByType("BaseCamera").at(idx)

            if(!keepCamera)
                viewAll();
        }
        noCamera.visible = root.camera && root.camera.sofaComponent ? true : false
    }


    Timer
    {
        id: rescanForCameraTimer
        interval: 500
        repeat: true
        onTriggered: {
            if (!cameraListModel.sofaScene)
                cameraListModel.sofaScene = root.sofaScene
            cameraListModel.update()
        }
    }

    Connections {
        target: sofaScene
        onStatusChanged: {
            if (SofaBaseScene.Ready === root.sofaScene.status) {
                cameraListModel.sofaScene = root.sofaScene;
            }
            if (SofaBaseScene.Unloading === root.sofaScene.status) {
                root.destroyCamera();
            }
        }
    }

    // screenshot / video
    function takeScreenshot(savePath) {
        if(undefined === savePath)
            savePath = "Captured/Screen/" + root.formatDateForScreenshot() + ".png";

        if(-1 === savePath.lastIndexOf("."))
            savePath += ".png";

        if(root.width.toFixed(0) === captureWidthTextField.text &&
                root.height.toFixed(0) === captureHeightTextField.text)
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

    // mouse interaction
    property alias mouseArea: mouseArea
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        enabled: sofaScene && sofaScene.ready

        onPressed: {
            forceActiveFocus();

            if(root.sofaScene)
                root.sofaScene.mousePressed(mouse, root);
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


        onActiveFocusChanged: {
            if(activeFocus) {
                SofaApplication.setFocusedSofaViewer(root);
            }
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

    property bool highlightIfFocused: SofaApplication.sofaViewers.length >= 1

    Rectangle {
        id: borderHighlighting
        anchors.fill: parent

        color: "transparent"
        border.width: 2
        border.color: "red"

        enabled: {
            if (root.highlightIfFocused) {
                if (root === SofaApplication.focusedSofaViewer)
                    return true;
                if (SofaApplication.focusedSofaViewer === null)
                    return true;
                return false
            }
            return false
        }
        visible: enabled

        onVisibleChanged: if(!visible) opacity = 0.0
        opacity: 0

        SequentialAnimation {
            id: animateBorder
            running: sofaScene.animate

            NumberAnimation {
                target: borderHighlighting
                property: "opacity"
                from: 1.0
                to: 0.2
                duration: 200
            }
            NumberAnimation {
                target: borderHighlighting
                property: "opacity"
                from: 1.0
                to: 0.2
                duration: 200
            }
            NumberAnimation {
                target: borderHighlighting
                property: "opacity"
                from: 1.0
                to: 0.0
                duration: 200
            }
            NumberAnimation {
                target: borderHighlighting
                property: "opacity"
                from: 1.0
                to: 0.0
                duration: 200
            }
        }
    }

    // toolpanel
    SidePanel {
        id: toolPanel
        z: 2
        control: Flickable {
            id: flick
            clip: true
            flickableDirection: Flickable.VerticalFlick
            boundsMovement: Flickable.StopAtBounds
            contentHeight: panelColumn.height + 50
            contentWidth: flick.width

            ScrollBar.horizontal: ScrollBar {
                policy: ScrollBar.AlwaysOff
            }

            ScrollBar.vertical: VerticalScrollbar {
                id: scrollbar
                content: panelColumn
            }

            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.topMargin: 5

            ColumnLayout {
                id: panelColumn
                width: scrollbar.visible ? parent.width - 12 : parent.width - 5
                spacing: 5

                Label {
                    Layout.fillWidth: true
                    text: "Camera ID"
                }

                ComboBox {
                    id: indexCameraComboBox
                    Layout.fillWidth: true
                    textRole: "name"

                    model: root.cameraListModel
                    onCurrentIndexChanged: {
                        root.recreateCamera(currentIndex)
                    }
                }
                GroupBox {
                    id: visualPanel
                    Layout.fillWidth: true
                    title: "Visual"

                    GridLayout {
                        anchors.fill: parent
                        columnSpacing: 0
                        rowSpacing: 2
                        columns: 2

                        Label {
                            Layout.fillWidth: true
                            text: "Antialiasing"
                        }

                        CheckBox {
                            id: antialiasingSwitch
                            Layout.alignment: Qt.AlignCenter
                            Component.onCompleted: checked = (0 !== root.antialiasingSamples)

                            onCheckedChanged: {
                                if(checked) {
                                    antialiasingSlider.uploadValue(antialiasingSlider.minimumValue);
                                    antialiasingLayout.visible = true;
                                } else {
                                    antialiasingLayout.visible = false;
                                    antialiasingSlider.uploadValue(0);
                                }
                            }

                            ToolTip {
                                description: "Enable / Disable Antialiasing\n\nNote : You must resize your window before the changes will take effect"
                            }
                        }

                        RowLayout {
                            id: antialiasingLayout
                            Layout.alignment: Qt.AlignCenter
                            Layout.columnSpan: 2

                            Slider {
                                id: antialiasingSlider
                                Layout.fillWidth: true
                                Component.onCompleted: downloadValue();
                                onValueChanged: if(visible) uploadValue(value);

                                stepSize: 1
                                from: 1
                                to: 4

                                function downloadValue() {
                                    value = Math.min((root.antialiasingSamples >= 1 ? Math.log(root.antialiasingSamples) / Math.log(2.0) : minimumValue), maximumValue);
                                }

                                function uploadValue(newValue) {
                                    if(undefined === newValue)
                                        newValue = value;

                                    root.antialiasingSamples = (newValue >= 1 ? Math.round(Math.pow(2.0, newValue)) : 0);
                                }

                                Connections {
                                    target: root
                                    onAntialiasingSamplesChanged: antialiasingSlider.downloadValue();
                                }

                                ToolTip {
                                    description: "Change the number of samples used for antialiasing\n\nNote : You must resize your window before the changes will take effect"
                                }
                            }

                            TextField {
                                Layout.preferredWidth: 32
                                readOnly: true
                                text: root.antialiasingSamples;
                            }
                        }
                    }
                }
                GroupBox {
                    id: savePanel
                    Layout.fillWidth: true

                    title: "Save"

                    GridLayout {
                        anchors.fill: parent
                        columnSpacing: 2
                        rowSpacing: 2
                        columns: 2

                        Item {
                            Layout.fillWidth: true
                            Layout.preferredHeight: screenshotButton.implicitHeight

                            Button {
                                id: screenshotButton
                                anchors.fill: parent
                                text: "Screenshot"
                                checked: false
                                checkable: false

                                onClicked: saveScreenshotDialog.open();

                                FileDialog {
                                    id: saveScreenshotDialog
                                    folder: "Captured/Screen/"
                                    selectExisting: false
                                    title: "Path to the screenshot to save"

                                    onAccepted: {
                                        var path = fileUrl.toString().replace("file://", "");
                                        root.takeScreenshot(path);
                                    }
                                }

                                ToolTip {
                                    description: "Save screenshot"
                                }
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.preferredHeight: movieButton.implicitHeight

                            Button {
                                id: movieButton
                                anchors.fill: parent
                                text: "Movie"
                                checked: false
                                checkable: true

                                onClicked: {
                                    if(checked)
                                        saveVideoDialog.open();
                                    else
                                        root.stopVideoRecording();
                                }

                                FileDialog {
                                    id: saveVideoDialog
                                    folder: "Captured/Movie/"
                                    selectExisting: false
                                    title: "Path to the movie to save"

                                    onAccepted: {
                                        root.startVideoRecording(fileUrl.toString().replace("file://", ""));
                                    }

                                    onRejected: {
                                        movieButton.checked = false;
                                    }
                                }

                                ToolTip {
                                    description: "Save video"
                                }
                            }
                        }

                        Label {
                            Layout.fillWidth: true
                            Layout.columnSpan: 2

                            text: "Resolution"
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "X"
                            }

                            TextField {
                                id: captureWidthTextField
                                Layout.fillWidth: true
                                validator: IntValidator {bottom: 1}

                                text: "1"
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: " Y"
                            }

                            TextField {
                                id: captureHeightTextField
                                Layout.fillWidth: true
                                validator: IntValidator {bottom: 1}

                                text: "1"
                            }
                        }

                        Connections {
                            target: root
                            onWidthChanged: updateResolutionTextFields();
                            onHeightChanged: updateResolutionTextFields();

                            function updateResolutionTextFields() {
                                captureWidthTextField.text = root.width.toFixed(0);
                                captureHeightTextField.text = root.height.toFixed(0);
                            }
                        }
                    }
                }
            }
        }
    }

    Item
    {
        anchors.fill: parent
        id: noCamera
        Image
        {
            anchors.centerIn: parent
            height: 30
            width: 30
            z: 0
            source:  "qrc:/icon/screenshot.png"
            opacity: 0.3
        }
        Image
        {
            anchors.centerIn: parent
            height: 60
            width: 60
            z: 1
            source:  "qrc:/icon/disabled.png"
            opacity: 0.3
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

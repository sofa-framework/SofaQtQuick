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
import SofaInteractors 1.0
import SofaViewer 1.0
import SofaScene 1.0

SofaViewer {
    id: root

    clip: true
    backgroundColor: SofaApplication.style.editviewBackgroundColor
//    backgroundImageSource: "qrc:/icon/sofaLogoAlpha.png"
    mirroredHorizontally: false
    mirroredVertically: false
    antialiasingSamples: 2
    sofaScene: SofaApplication.sofaScene
    property bool configurable: true


   property bool transparentBackground: false
    Image {
        anchors.fill: parent
        z: -1
        visible: !root.transparentBackground && root.backgroundColor.a < 1.0
        fillMode: Image.Tile
        source: "qrc:/icon/alphaBackground.png"
    }

    implicitWidth: 800
    implicitHeight: 600

    Component.onCompleted: {
        SofaApplication.addSofaViewer(root);

        if(!SofaApplication.focusedSofaViewer)
            forceActiveFocus();

        if(root.sofaScene && root.sofaScene.ready)
            recreateCamera();

        loadCameraFromFile((Number(uiId)));
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

	Action{
		shortcut: "F5"
        onTriggered: {
            root.viewAll()
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

        Camera {

        }
    }

    property bool defaultCameraOrthographic: false
    property bool keepCamera: false
    property bool hideBusyIndicator: false

    function recreateCamera() {
        if(camera && !keepCamera) {
            camera.destroy();
            camera = null;
        }

        if(!camera) {
            camera = cameraComponent.createObject(root, {orthographic: defaultCameraOrthographic} );

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

// interactor

    property alias interactor: interactorLoader.item
    property Component interactorComponent: SofaApplication.interactorComponent

    Loader {
        id: interactorLoader
        sourceComponent: root.interactorComponent
//            source: "qrc:/SofaInteractors/UserInteractor_MoveCamera.qml"
        onLoaded: {
            var interactor = item;
            interactor.init();
        }
    }

    Image {
        id: handIcon
        source: "qrc:/icon/hand.png"
        visible: sofaScene && sofaScene.sofaParticleInteractor ? sofaScene.sofaParticleInteractor.interacting : false
        antialiasing: true

        Connections {
            target: sofaScene && sofaScene.sofaParticleInteractor ? sofaScene.sofaParticleInteractor : null
            onInteractorPositionChanged: {
                var position = root.mapFromWorld(sofaScene.sofaParticleInteractor.interactorPosition)
                if(position.z > 0.0 && position.z < 1.0) {
                    handIcon.x = position.x - 6;
                    handIcon.y = position.y - 2;
                }
            }
        }
    }

// mouse interaction

    property alias mouseArea: mouseArea
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        enabled: sofaScene && sofaScene.ready
        hoverEnabled: root.interactor ? root.interactor.hoverEnabled : false

        onClicked: {
            forceActiveFocus();

            if(root.interactor)
                root.interactor.mouseClicked(mouse, root);

        }

        onDoubleClicked: {
            forceActiveFocus();

            if(root.interactor)
                root.interactor.mouseDoubleClicked(mouse, root);
        }

        onPressed: {
            forceActiveFocus();

            if(root.interactor)
                root.interactor.mousePressed(mouse, root);
        }

        onReleased: {
            if(root.interactor)
                root.interactor.mouseReleased(mouse, root);
        }

        onWheel: {
            if(root.interactor)
                root.interactor.mouseWheel(wheel, root);

            wheel.accepted = true;
        }

        onPositionChanged: {
            if(root.interactor)
                root.interactor.mouseMoved(mouse, root);
        }
    }

// keyboard interaction

    Keys.onPressed: {
        if(event.isAutoRepeat) {
            event.accepted = true;
            return;
        }

        if(root.sofaScene)
            root.sofaScene.keyPressed(event);

        if(root.interactor)
            root.interactor.keyPressed(event, root);

        event.accepted = true;
    }

    Keys.onReleased: {
        if(event.isAutoRepeat) {
            event.accepted = true;
            return;
        }

        if(root.sofaScene)
            root.sofaScene.keyReleased(event);

        if(root.interactor)
            root.interactor.keyReleased(event, root);

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

    /*Item {
        id: circleGizmo
        anchors.centerIn: parent
        visible: false

        opacity: 0.75
        property color color: "red"
        property real size: Math.min(root.width, root.height) / 2.0
        property real thickness: 1

        Rectangle {
            anchors.centerIn: parent
            color: "transparent"
            border.color: circleGizmo.color
            border.width: circleGizmo.thickness
            width: circleGizmo.size
            height: width
            radius: width / 2.0
        }
    }*/

// toolpanel

    Rectangle {
        id: toolPanel
        color: "lightgrey"
        anchors.top: toolPanelSwitch.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.topMargin: -6
        anchors.bottomMargin: 20
        anchors.rightMargin: -radius
        width: 250
        radius: 5
        visible: false
        opacity: 0.9

        // avoid mouse event propagation through the toolpanel to the sofa viewer
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            onWheel: wheel.accepted = true
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: toolPanel.radius / 2
            anchors.rightMargin: anchors.margins - toolPanel.anchors.rightMargin
            spacing: 2

            Text {
                Layout.fillWidth: true
                text: "SofaViewer parameters"
                font.bold: true
                color: "darkblue"
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Flickable {
                    anchors.fill: parent
                    contentHeight: panelColumn.implicitHeight

                    Column {
                        id: panelColumn
                        anchors.fill: parent
                        spacing: 5

                        GroupBox {
                            id: visualPanel
                            implicitWidth: parent.width
                            title: "Visual"

                            GridLayout {
                                anchors.fill: parent
                                columnSpacing: 0
                                rowSpacing: 2
                                columns: 2

    // antialiasing

                                Label {
                                    Layout.fillWidth: true
                                    text: "Antialiasing"
                                }

                                Switch {
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

    // background

                                Label {
                                    Layout.fillWidth: true
                                    text: "Background"
                                }

                                Rectangle {
                                    Layout.preferredWidth: 48
                                    Layout.preferredHeight: 20
                                    Layout.alignment: Qt.AlignCenter
                                    color: "darkgrey"
                                    radius: 2

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: backgroundColorPicker.open()
                                    }

                                    ColorDialog {
                                        id: backgroundColorPicker
                                        title: "Please choose a background color"
                                        showAlphaChannel: true

                                        property color previousColor
                                        Component.onCompleted: {
                                            previousColor = root.backgroundColor;
                                            color = previousColor;
                                        }

                                        onColorChanged: root.backgroundColor = color

                                        onAccepted: previousColor = color
                                        onRejected: color = previousColor
                                    }

                                    Rectangle {
                                        anchors.fill: parent
                                        anchors.margins: 2
                                        color: Qt.rgba(root.backgroundColor.r, root.backgroundColor.g, root.backgroundColor.b, 1.0)

                                        ToolTip {
                                            description: "Background color"
                                        }
                                    }
                                }

    // background

                                Label {
                                    Layout.fillWidth: true
                                    text: "Frame"
                                }

                                Switch {
                                    id: frameSwitch
                                    Layout.alignment: Qt.AlignCenter
                                    Component.onCompleted: checked = root.drawFrame

                                    onCheckedChanged: root.drawFrame = checked;

                                    ToolTip {
                                        description: "Enable / Disable Scene Frame"
                                    }
                                }
                            }
                        }

    // save screenshot / movie

                        GroupBox {
                            id: savePanel
                            implicitWidth: parent.width

                            title: "Save"

                            GridLayout {
                                anchors.fill: parent
                                columnSpacing: 2
                                rowSpacing: 2
                                columns: 2

        // screenshot

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

        // movie

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

    // camera

                        GroupBox {
                            id: cameraPanel
                            implicitWidth: parent.width
                            implicitHeight: cameraLayout.implicitHeight

                            title: "Camera"

                            Column {
                                id: cameraLayout
                                anchors.fill: parent
                                spacing: 0

                                GroupBox {
                                    implicitWidth: parent.width
                                    title: "Mode"
//                                    flat: true

                                    RowLayout {
                                        anchors.fill: parent
                                        spacing: 0

        // orthographic

                                        Button {
                                            id: orthoButton
                                            Layout.fillWidth: true
                                            Layout.preferredWidth: parent.width

                                            text: "Orthographic"
                                            checkable: true
                                            checked: root.camera ? root.camera.orthographic : false
                                            onCheckedChanged: if(root.camera && checked !== root.camera.orthographic) root.camera.orthographic = checked;

                                            Connections {
                                                target: root
                                                onCameraChanged: orthoButton.update();
                                            }

                                            Connections {
                                                target: root.camera
                                                onOrthographicChanged: orthoButton.update();
                                            }

                                            function update() {
                                                if(!root.camera) {
                                                    checked = false;
                                                    return;
                                                }

                                                if(orthoButton.checked !== root.camera.orthographic)
                                                    orthoButton.checked = root.camera.orthographic;
                                            }

                                            ToolTip {
                                                description: "Orthographic Mode"
                                            }
                                        }

        // perspective

                                        Button {
                                            id: perspectiveButton
                                            Layout.fillWidth: true
                                            Layout.preferredWidth: parent.width

                                            text: "Perspective"
                                            checkable: true
                                            checked: root.camera ? !camera.orthographic : true
                                            onCheckedChanged: if(root.camera && checked === root.camera.orthographic) root.camera.orthographic = !checked;

                                            Connections {
                                                target: root
                                                onCameraChanged: perspectiveButton.update();
                                            }

                                            Connections {
                                                target: root.camera
                                                onOrthographicChanged: perspectiveButton.update();
                                            }

                                            function update() {
                                                if(!root.camera) {
                                                    checked = false;
                                                    return;
                                                }

                                                if(perspectiveButton.checked !== !root.camera.orthographic)
                                                    perspectiveButton.checked = !root.camera.orthographic;
                                            }

                                            ToolTip {
                                                description: "Perspective Mode"
                                            }
                                        }
                                    }
                                }

                                GroupBox {
                                    implicitWidth: parent.width
                                    title: "Depth"
//                                    flat: true

                                    RowLayout {
                                        anchors.fill: parent

        // near

                                        Item {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: zNearLayout.implicitHeight

                                            RowLayout {
                                                id: zNearLayout
                                                anchors.fill: parent

                                                Label {
                                                    text: "Near"
                                                    Layout.preferredWidth: 30
                                                }

                                                TextField {
                                                    id: zNearTextField
                                                    Layout.fillWidth: true
                                                    implicitWidth: 200

                                                    enabled: root.camera

                                                    Component.onCompleted: download();
                                                    onAccepted: {
                                                        upload();
                                                        download();
                                                    }

                                                    validator: DoubleValidator {}

                                                    Connections {
                                                        target: root.camera
                                                        onZNearChanged: zNearTextField.download();
                                                    }

                                                    function download() {
                                                        if(!root.camera)
                                                            return;

                                                        zNearTextField.text = Number(root.camera.zNear).toString();
                                                        cursorPosition = 0;
                                                    }

                                                    function upload() {
                                                        var oldValue = Number(root.camera.zNear);
                                                        var newValue = Number(zNearTextField.text);

                                                        if(oldValue !== newValue)
                                                            root.camera.zNear = newValue;
                                                    }

                                                    ToolTip {
                                                        description: "Depth of the camera near plane"
                                                    }
                                                }
                                            }
                                        }

        // far

                                        Item {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: zFarLayout.implicitHeight

                                            RowLayout {
                                                id: zFarLayout
                                                anchors.fill: parent

                                                TextField {
                                                    id: zFarTextField
                                                    Layout.fillWidth: true
                                                    implicitWidth: 200

                                                    enabled: root.camera

                                                    Component.onCompleted: download();
                                                    onAccepted: {
                                                        upload();
                                                        download();
                                                    }

                                                    validator: DoubleValidator {}

                                                    Connections {
                                                        target: root.camera
                                                        onZFarChanged: zFarTextField.download();
                                                    }

                                                    function download() {
                                                        if(!root.camera)
                                                            return;

                                                        zFarTextField.text = Number(root.camera.zFar).toString();
                                                        cursorPosition = 0;
                                                    }

                                                    function upload() {
                                                        var oldValue = Number(root.camera.zFar);
                                                        var newValue = Number(zFarTextField.text);

                                                        if(oldValue !== newValue)
                                                            root.camera.zFar = newValue;
                                                    }

                                                    ToolTip {
                                                        description: "Depth of the camera far plane"
                                                    }
                                                }

                                                Label {
                                                    text: "Far"
                                                    Layout.preferredWidth: 30
                                                }
                                            }
                                        }
                                    }
                                }


                                //                                Label {
                                //                                    Layout.fillWidth: true
                                //                                    text: "Logo"
                                //                                }

                                //                                RowLayout {
                                //                                    Layout.fillWidth: true
                                //                                    spacing: 0

                                //                                    TextField {
                                //                                        id: logoTextField
                                //                                        Layout.fillWidth: true
                                //                                        Component.onCompleted: text = root.backgroundImageSource
                                //                                        onAccepted: root.backgroundImageSource = text
                                //                                    }

                                //                                    Button {
                                //                                        Layout.preferredWidth: 22
                                //                                        Layout.preferredHeight: Layout.preferredWidth
                                //                                        iconSource: "qrc:/icon/open.png"

                                //                                        onClicked: openLogoDialog.open()

                                //                                        FileDialog {
                                //                                            id: openLogoDialog
                                //                                            title: "Please choose a logo"
                                //                                            selectFolder: true
                                //                                            selectMultiple: false
                                //                                            selectExisting: true
                                //                                            property var resultTextField
                                //                                            onAccepted: {
                                //                                                logoTextField.text = Qt.resolvedUrl(fileUrl)
                                //                                                logoTextField.accepted();
                                //                                            }
                                //                                        }
                                //                                    }
                                //                                }

    // view

                                GroupBox {
                                    implicitWidth: parent.width
                                    title: "View"
//                                    flat: true

                                    GridLayout {
                                        anchors.fill: parent
                                        columns: 2
                                        rowSpacing: 0
                                        columnSpacing: 0
                                       
                                       Button {
                                            Layout.fillWidth: true
                                            Layout.preferredWidth: parent.width
                                            text: "Save"

                                            onClicked: if(camera) root.saveCameraToFile(Number(uiId))

                                            ToolTip {
                                                description: "Save the current view to a sidecar file"
                                            }
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            Layout.preferredWidth: parent.width
                                            text: "Reload"

                                            onClicked: if(camera) root.loadCameraFromFile(Number(uiId))

                                            ToolTip {
                                                description: "Reload the view from data contained in a sidecar file (if present)"
                                            }
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            Layout.columnSpan: 2
                                            text: "Fit"

                                            onClicked: if(camera) camera.fit(root.boundingBoxMin(), root.boundingBoxMax())

                                            ToolTip {
                                                description: "Fit in view"
                                            }
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            text: "-X"

                                            onClicked: if(camera) camera.viewFromLeft()

                                            ToolTip {
                                                description: "Align view along the negative X Axis"
                                            }
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            text: "+X"

                                            onClicked: if(camera) camera.viewFromRight()

                                            ToolTip {
                                                description: "Align view along the positive X Axis"
                                            }
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            text: "-Y"

                                            onClicked: if(camera) camera.viewFromTop()

                                            ToolTip {
                                                description: "Align view along the negative Y Axis"
                                            }
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            text: "+Y"

                                            onClicked: if(camera) camera.viewFromBottom()

                                            ToolTip {
                                                description: "Align view along the positive Y Axis"
                                            }
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            Layout.preferredWidth: parent.width
                                            text: "-Z"

                                            onClicked: if(camera) camera.viewFromFront()

                                            ToolTip {
                                                description: "Align view along the negative Z Axis"
                                            }
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            Layout.preferredWidth: parent.width
                                            text: "+Z"

                                            onClicked: if(camera) camera.viewFromBack()

                                            ToolTip {
                                                description: "Align view along the positive Z Axis"
                                            }
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            Layout.columnSpan: 2
                                            text: "Isometric"

                                            onClicked: if(camera) camera.viewIsometric()

                                            ToolTip {
                                                description: "Isometric View"
                                            }
                                        } 

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Image {
        id: toolPanelSwitch
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 26
        anchors.rightMargin: 3
        source: toolPanel.visible ? "qrc:/icon/minus.png" : "qrc:/icon/plus.png"
        width: 12
        height: width
        visible: configurable

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: toolPanel.visible = !toolPanel.visible
        }
    }
}

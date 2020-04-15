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
import SofaApplication 1.0
import SofaInteractors 1.0
import EditView 1.0
import SofaColorScheme 1.0
import SofaBasics 1.0
import SofaWidgets 1.0
import SofaManipulators 1.0
import SofaInteractors 1.0

EditView
{
    readonly property string docstring :
        "This view is rendering your scene using a camera controlled by the sofa application you are
         using. It is possible to edit or move the camera location in this view ."

    id: root
    clip: true
    backgroundColor: "#FF404040"
    mirroredHorizontally: false
    mirroredVertically: false
    antialiasingSamples: 2
    sofaScene: SofaApplication.sofaScene

    property bool transparentBackground: false
    Image {
        anchors.fill: parent
        z: -1
        visible: !root.transparentBackground && root.backgroundColor.a < 1.0
        fillMode: Image.Tile
        source: "qrc:/icon/alphaBackground.png"
    }

    property int uiId: 0
    property int previousUiId: uiId
    onUiIdChanged: {
        SofaApplication.uiSettings.replace(previousUiId, uiId);
    }

    implicitWidth: 800
    implicitHeight: 600

    Component.onCompleted: {
        SofaApplication.addSofaViewer(root);

        if(!SofaApplication.focusedSofaView)
            forceActiveFocus();

        if(root.sofaScene && root.sofaScene.ready)
            recreateCamera();

        loadCameraFromFile((Number(uiId)));
    }

    Component.onDestruction: {
        SofaApplication.removeSofaViewer(root);
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

    // camera

    Component {
        id: cameraComponent

        Camera {

        }
    }





    //    // screenshot / video

    //    function takeScreenshot(savePath) {
    //        if(undefined === savePath)
    //            savePath = "Captured/Screen/" + root.formatDateForScreenshot() + ".png";

    //        if(-1 === savePath.lastIndexOf("."))
    //            savePath += ".png";

    //        if(root.width.toFixed(0) == captureWidthTextField.text && root.height.toFixed(0) == captureHeightTextField.text)
    //            root.saveScreenshot(savePath);
    //        else
    //            root.saveScreenshotWithResolution(savePath, Number(captureWidthTextField.text), Number(captureHeightTextField.text));
    //    }

    //    function startVideoRecording(savePath) {
    //        videoRecordingPrivate.videoPath = savePath;
    //        videoRecordingPrivate.saveVideo = true;
    //    }

    //    function stopVideoRecording() {
    //        videoRecordingPrivate.saveVideo = false;
    //    }

    //    property var _videoRecordingPrivate: QtObject {
    //        id: videoRecordingPrivate

    //        property string videoPath: ""
    //        property bool saveVideo: false
    //        onSaveVideoChanged: {
    //            if(!saveVideo)
    //                return;

    //            videoFrameNumber = 0;

    //            saveVideoFrame();
    //        }

    //        property int videoFrameNumber: 0
    //        property var sceneConnections: Connections {
    //            target: root.sofaScene && videoRecordingPrivate.saveVideo ? root.sofaScene : null
    //            onStepEnd: videoRecordingPrivate.saveVideoFrame();
    //        }

    //        function saveVideoFrame() {
    //            var savePath = videoRecordingPrivate.videoPath;
    //            if(0 === savePath.length)
    //                savePath = "Captured/Movie/" + SofaApplication.formatDateForScreenshot() + "/movie.png";

    //            var dotIndex = savePath.lastIndexOf(".");
    //            if(-1 === dotIndex) {
    //                savePath += ".png"
    //                dotIndex = savePath.lastIndexOf(".");
    //            }

    //            savePath = savePath.slice(0, dotIndex) + "_" + (videoRecordingPrivate.videoFrameNumber++).toString() + savePath.slice(dotIndex);

    //            root.saveScreenshot(savePath);
    //        }
    //    }

    Action{
        shortcut: "F5"
        onTriggered: root.viewAll()
    }

    //    Image {
    //        id: handIcon
    //        source: "qrc:/icon/hand.png"
    //        visible: sofaScene && sofaScene.sofaParticleInteractor ? sofaScene.sofaParticleInteractor.interacting : false
    //        antialiasing: true

    //        Connections {
    //            target: sofaScene && sofaScene.sofaParticleInteractor ? sofaScene.sofaParticleInteractor : null
    //            onInteractorPositionChanged: {
    //                var position = root.mapFromWorld(sofaScene.sofaParticleInteractor.interactorPosition)
    //                if(position.z > 0.0 && position.z < 1.0) {
    //                    handIcon.x = position.x - 6;
    //                    handIcon.y = position.y - 2;
    //                }
    //            }
    //        }
    //    }

    // mouse interaction forwarding to the interactor
    property alias mouseArea: mouseArea
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        enabled: sofaScene && sofaScene.ready
        hoverEnabled: SofaApplication.interactorComponent.hoverEnabled
        preventStealing: true

        onClicked: {
            forceActiveFocus();

            SofaApplication.interactorComponent.mouseClicked(mouse, root);

        }

        onDoubleClicked: {
            forceActiveFocus();

            SofaApplication.interactorComponent.mouseDoubleClicked(mouse, root);
        }

        onPressed: {
            forceActiveFocus();

            SofaApplication.interactorComponent.mousePressed(mouse, root);
        }

        onReleased: {
            SofaApplication.interactorComponent.mouseReleased(mouse, root);
        }

        onWheel: {
            SofaApplication.interactorComponent.mouseWheel(wheel, root);

            wheel.accepted = true;
        }

        onPositionChanged: {
            SofaApplication.interactorComponent.mouseMoved(mouse, root);
        }

        onActiveFocusChanged: {
            if(activeFocus) {
                SofaApplication.setFocusedSofaViewer(root);
            }
        }
    }

    // keyboard interaction
    Keys.onPressed: {
        if(event.isAutoRepeat) {
            event.accepted = true;
            return;
        }

        SofaApplication.interactorComponent.keyPressed(event, root);

        event.accepted = true;
    }

    Keys.onReleased: {
        if(event.isAutoRepeat) {
            event.accepted = true;
            return;
        }

        SofaApplication.interactorComponent.keyReleased(event, root);

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
    SidePanel {
        id: toolPanel
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


                GroupBox {
                    id: visualPanel
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

                        CheckBox {
                            id: antialiasingSwitch
                            Layout.alignment: Qt.AlignRight
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
                                text: "Enable / Disable Antialiasing"
                                description: "Note : You must resize your window before the changes will take effect"
                            }
                        }

                        RowLayout {
                            id: antialiasingLayout
                            Layout.alignment: Qt.AlignCenter
                            Layout.columnSpan: 2
                            Layout.fillWidth: true

                            Slider {
                                id: antialiasingSlider
                                Layout.fillWidth: true
                                Component.onCompleted: downloadValue();
                                onValueChanged: if(visible) uploadValue(value);

                                stepSize: 1
                                from: 1
                                to: 4

                                function downloadValue() {
                                    value = Math.min((root.antialiasingSamples >= 1 ? Math.log(root.antialiasingSamples) / Math.log(2.0) : from), to);
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
                                    text: "Change the number of samples used for antialiasing"
                                    description: "Note : You must resize your window before the changes will take effect"
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
                            Layout.alignment: Qt.AlignRight
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
                                    text: "Background color"
                                }
                            }
                        }
                        // background
                        Label {
                            Layout.fillWidth: true
                            text: "Frame"
                        }

                        CheckBox {
                            id: frameSwitch
                            Layout.alignment: Qt.AlignRight
                            Component.onCompleted: checked = root.drawFrame

                            onCheckedChanged: root.drawFrame = checked;

                            ToolTip {
                                text: "Enable / Disable Scene Frame"
                            }
                        }
                        Label {
                            Layout.fillWidth: true
                            text: "units: "
                        }
                        ComboBox {
                            model: ["Milimetres (mm)", "Centimeters (cm)", "Decimeters (dm)", "Meters (m)", "Decameters (dam)", "Hectometers (hm)", "Kilometers (km)"]
                            currentIndex: 3
                            Component.onCompleted: {
                                sceneUnits = 1
                            }
                            onCurrentTextChanged: {
                                print( currentText)
                                if (currentText.includes("(mm)"))
                                    sceneUnits = 1000
                                if (currentText.includes("(cm)"))
                                    sceneUnits = 100
                                if (currentText.includes("(dm)"))
                                    sceneUnits = 10
                                if (currentText.includes("(m)"))
                                    sceneUnits = 1
                                if (currentText.includes("(dam)"))
                                    sceneUnits = 0.1
                                if (currentText.includes("(hm)"))
                                    sceneUnits = 0.01
                                if (currentText.includes("(km)"))
                                    sceneUnits = 0.001
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
                        columnSpacing: -1
                        //                                rowSpacing: -1
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
                                position: cornerPositions['Left']

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
                                    text: "Save screenshot"
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
                                position: cornerPositions['Right']

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
                                    text: "Save video"
                                }
                            }
                        }

                        Label {
                            Layout.fillWidth: true
                            Layout.columnSpan: 2

                            text: "Resolution"
                        }

                        RowLayout {

                            //                                    Label {
                            //                                        text: "X"
                            //                                    }

                            TextField {
                                id: captureWidthTextField
                                Layout.fillWidth: true
                                validator: IntValidator {bottom: 1}
                                position: cornerPositions['Left']

                                text: "1"
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            //                                    Label {
                            //                                        text: " Y"
                            //                                    }

                            TextField {
                                id: captureHeightTextField
                                Layout.fillWidth: true
                                validator: IntValidator {bottom: 1}
                                position: cornerPositions['Right']

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
                                spacing: -1

                                // orthographic

                                Button {
                                    id: orthoButton
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: parent.width
                                    position: cornerPositions['Left']

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
                                        text: "Orthographic Mode"
                                    }
                                }

                                // perspective

                                Button {
                                    id: perspectiveButton
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: parent.width
                                    position: cornerPositions['Right']

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
                                        text: "Perspective Mode"
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
                                spacing: -1

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
                                            position: cornerPositions['Left']

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
                                                text: "Depth of the camera near plane"
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
                                            position: cornerPositions['Right']

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
                                                text: "Depth of the camera far plane"
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
                                rowSpacing: -1
                                columnSpacing: -1

                                Button {
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: parent.width
                                    text: "Save"
                                    position: cornerPositions['Left']
                                    onClicked: if(camera) root.saveCameraToFile(Number(uiId))

                                    ToolTip {
                                        text: "Save the current view to a sidecar file"
                                    }
                                }

                                Button {
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: parent.width
                                    text: "Reload"
                                    position: cornerPositions['Right']

                                    onClicked: if(camera) root.loadCameraFromFile(Number(uiId))

                                    ToolTip {
                                        text: "Reload the view from data contained in a sidecar file (if present)"
                                    }
                                }
                                Rectangle {
                                    color: "transparent"
                                    height: 5
                                }

                                Button {
                                    Layout.fillWidth: true
                                    Layout.columnSpan: 2
                                    text: "Fit"
                                    position: cornerPositions['Top']

                                    onClicked: if(camera) camera.fit(root.boundingBoxMin(), root.boundingBoxMax())

                                    ToolTip {
                                        text: "Fit in view"
                                    }
                                }

                                Button {
                                    Layout.fillWidth: true
                                    text: "-X"
                                    position: cornerPositions['Middle']

                                    onClicked: if(camera) camera.viewFromLeft()

                                    ToolTip {
                                        text: "Align view along the negative X Axis"
                                    }
                                }

                                Button {
                                    Layout.fillWidth: true
                                    text: "+X"
                                    position: cornerPositions['Middle']

                                    onClicked: if(camera) camera.viewFromRight()

                                    ToolTip {
                                        text: "Align view along the positive X Axis"
                                    }
                                }

                                Button {
                                    Layout.fillWidth: true
                                    text: "-Y"
                                    position: cornerPositions['Middle']

                                    onClicked: if(camera) camera.viewFromTop()

                                    ToolTip {
                                        text: "Align view along the negative Y Axis"
                                    }
                                }

                                Button {
                                    Layout.fillWidth: true
                                    text: "+Y"
                                    position: cornerPositions['Middle']

                                    onClicked: if(camera) camera.viewFromBottom()

                                    ToolTip {
                                        text: "Align view along the positive Y Axis"
                                    }
                                }

                                Button {
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: parent.width
                                    text: "-Z"
                                    position: cornerPositions['Middle']

                                    onClicked: if(camera) camera.viewFromFront()

                                    ToolTip {
                                        text: "Align view along the negative Z Axis"
                                    }
                                }

                                Button {
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: parent.width
                                    text: "+Z"
                                    position: cornerPositions['Middle']

                                    onClicked: if(camera) camera.viewFromBack()

                                    ToolTip {
                                        text: "Align view along the positive Z Axis"
                                    }
                                }

                                Button {
                                    Layout.fillWidth: true
                                    Layout.columnSpan: 2
                                    text: "Isometric"
                                    position: cornerPositions['Bottom']

                                    onClicked: if(camera) camera.viewIsometric()

                                    ToolTip {
                                        text: "Isometric View"
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    IconComboBox {
        id: modes
        model: ListModel {
            id: listmodel
            ListElement {
                name: "Object Mode"
                image: "qrc:/icon/ICON_OBJECT_MODE.png"
            }
            ListElement {
                name: "Edit Mode"
                image: "qrc:/icon/ICON_EDIT_MODE.png"
            }
        }

        UserInteractor_EditMode {
            id:  editModeInteractor
            Component.onCompleted: {
                editModeInteractor.init()

                var m = getManipulator("Viewpoint_Manipulator")
                if (m !== null) {
                    m.persistent = true
                    m.enabled = true
                }
            }
        }
        UserInteractor_ObjectMode {
            id: objectModeInteractor
            Component.onCompleted: {
                objectModeInteractor.init()

                var m = getManipulator("Viewpoint_Manipulator")
                if (m !== null) {
                    m.persistent = true
                    m.enabled = true
                }
            }
        }

        onCurrentIndexChanged: {
            switch (currentIndex) {
            case 0:
                manipulatorLoader.sourceComponent = objectmodeManipulators
                SofaApplication.interactorComponent = objectModeInteractor
                break;
            case 1:
                manipulatorLoader.sourceComponent = editmodeManipulators
                SofaApplication.interactorComponent = editModeInteractor
                break;
            default:
                manipulatorLoader.sourceComponent = objectmodeManipulators
                SofaApplication.interactorComponent = objectModeInteractor
            }
        }
    }

    Loader {
        id: manipulatorLoader
        sourceComponent: objectmodeManipulators
        onLoaded: {
            SofaApplication.selectedManipulator = null
        }
    }

    Component {
        id: objectmodeManipulators
        ObjectModeManipulators {
            sofaViewer: root
        }
    }

    Component {
        id: editmodeManipulators
        EditModeManipulators {
            sofaViewer: root
        }
    }

}

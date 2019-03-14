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
import SofaScene 1.0
import QMLUIArea 1.0

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

    Component.onCompleted: {
        SofaApplication.addSofaViewer(root);

        if(!SofaApplication.focusedSofaViewer)
            forceActiveFocus();

        if(root.sofaScene && root.sofaScene.ready)
            recreateCamera();

        //fill combobox
        var cameraList = sofaScene.componentsByType("BaseCamera")
        idComboList.clear()
        for (var i = 0; i < cameraList.size(); ++i)
        {
            idComboList.append({text: cameraList.at(i).getName()})
        }
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

        onWheel: {
            if(root.sofaScene)
                root.sofaScene.mouseWheel(wheel, root);

            wheel.accepted = true;
        }

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
                text: "CameraView parameters"
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
                            id: idCameraPanel
                            implicitWidth: parent.width
                            title: "ID"

                            GridLayout {
                                anchors.fill: parent
                                columnSpacing: 0
                                rowSpacing: 2
                                columns: 2

                                Label {
                                    Layout.fillWidth: true
                                    text: "Camera"
                                }

                                RowLayout {
                                    id: idCameraLayout
                                    Layout.alignment: Qt.AlignCenter
                                    Layout.columnSpan: 2

                                    ComboBox {
                                        id: indexCameraComboBox
                                        Layout.fillWidth: true

                                        model: root.idComboList

                                        onCurrentIndexChanged : switchCamera();

                                        function switchCamera() {
                                            //onsole.log("Change to " + indexViewerComboBox.currentIndex);
                                            root.updateCameraFromIndex(indexCameraComboBox.currentIndex);
                                            //root.xraymodel.update()
                                        }
                                    }
                                }
                            }
                        }
                        GroupBox {
                            id: visualPanel
                            implicitWidth: parent.width
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
                            }
                        }
                        GroupBox {
                            id: savePanel
                            implicitWidth: parent.width

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

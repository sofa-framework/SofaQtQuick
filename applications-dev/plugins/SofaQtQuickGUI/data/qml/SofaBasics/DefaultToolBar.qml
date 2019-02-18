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
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import SofaApplication 1.0
import SofaBasics 1.0

ToolBar {
    id: root
    height: 25

    property var sofaScene: SofaApplication.sofaScene

    Component.onCompleted: {
        SofaApplication.toolBar = root;
    }

    Component.onDestruction: {
        if(root === SofaApplication.toolBar)
            SofaApplication.toolBar = null;
    }

    enabled: sofaScene ? sofaScene.ready : false

    Row {
        id: mainrow
        spacing: 5
        anchors.fill: parent

        Text {
            text: "Interaction"
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            color: "steelblue"
        }

        Row {
            id: interactorPositioner

            Component {
                id: interactorButtonComponent

                ToolButton {
                    id: interactorButton
                    property string interactorName
                    property Component interactorComponent

                    height: root.height - 3
                    width: implicitWidth + 10

                    text: interactorName
                    checkable: true
                    checked: false
                    onCheckedChanged: if(checked) SofaApplication.interactorComponent = interactorComponent
                    onClicked: checked = true

                    Connections {
                        target: SofaApplication
                        onInteractorNameChanged: interactorButton.update();
                    }

                    Component.onCompleted: update();
                    function update() {
                        if(interactorName === SofaApplication.interactorName)
                            interactorButton.checked = true;
                        else
                            interactorButton.checked = false;
                    }
                }
            }

            Connections {
                target: SofaApplication
                onInteractorComponentMapChanged: interactorPositioner.update();
            }

            function update() {
                for(var i = 0; i < children.length; ++i)
                    children[i].destroy();

                var interactorComponentMap = SofaApplication.interactorComponentMap;
                for(var key in interactorComponentMap)
                    if(interactorComponentMap.hasOwnProperty(key)) {
                        var incubator = interactorButtonComponent.incubateObject(interactorPositioner, {interactorName: key, interactorComponent: interactorComponentMap[key]});
                        incubator.forceCompletion();
                    }
            }
        }

        Row {
            id: simulationControlTools
            spacing: 5

            ToolButton {
                id: animateButton
                height: root.height - 3

                iconSource: animateButton.checked ? "qrc:/icon/stopButton.png" : "qrc:/icon/playButton.png"
//                ToolTip.text: animateButton.checked ? "Stop" : "Animate"
                checkable: true
                checked: false
                onCheckedChanged: if(sofaScene) sofaScene.animate = animateButton.checked

                Connections {
                    target: sofaScene
                    ignoreUnknownSignals: true
                    onAnimateChanged: {
                        animateButton.checked = sofaScene.animate;
                    }
                }
            }

            ToolButton {
                id: stepButton
                height: root.height - 3

                iconSource: "qrc:/icon/stepButton.png"
//                ToolTip.text: "Step"

                onClicked: {
                    if(sofaScene)
                        sofaScene.step();
                }
            }

            ToolButton {
                id: resetButton
                height: root.height - 3

                iconSource: "qrc:/icon/resetButton.png"
//                ToolTip.text: "Reset the sofa scene"

                onClicked: {
                    if(sofaScene)
                        sofaScene.reset();
                }
            }

            Row {
                Label {
                    text: "DT (s)"
                    anchors.verticalCenter: parent.verticalCenter
                }
                TextField {
                    id: dtSpinBox
                    height: root.height - 3
                    Layout.preferredWidth: 68
                    validator: DoubleValidator {bottom: 0}
                    text: root.sofaScene ? root.sofaScene.dt.toString() : Number(0.04).toString()

                    onAccepted: {
                        if(root.sofaScene) root.sofaScene.dt = Number(text);
                        if(0 === Number(text))
                            text = "Real-time";
                    }
                }
            }
        }

        Row {
            id: captureLayout
            spacing: 5

            ToolButton {
                id: screenshotButton
                iconSource: "qrc:/icon/screenshot.png"
                height: root.height - 5

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
                        SofaApplication.takeScreenshot(path);
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    visible: screenshotButton.checked
                    z: -1
                    radius: 5
                    color: "white"
                }

//                ToolTip.text: "Save screenshot"
            }

            ToolButton {
                id: movieButton
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                iconSource: "qrc:/icon/movieRecording.png"

                checked: false
                checkable: true

                onClicked: {
                    if(checked)
                        saveVideoDialog.open();
                    else
                        SofaApplication.stopVideoRecording();
                }

                FileDialog {
                    id: saveVideoDialog
                    folder: "Captured/Movie/"
                    selectExisting: false
                    title: "Path to the movie to save"

                    onAccepted: {
                        SofaApplication.startVideoRecording(fileUrl.toString().replace("file://", ""));
                    }

                    onRejected: {
                        movieButton.checked = false;
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    visible: movieButton.checked
                    z: -1
                    radius: 5
                    color: "white"
                }

//                ToolTip.text: "Save video"
            }
        }
    }
}

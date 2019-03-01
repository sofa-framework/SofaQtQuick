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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import SofaApplication 1.0
import SofaBasics 1.0
import SofaColorScheme 1.0

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
        anchors.verticalCenter: parent.verticalCenter
        id: mainrow
        spacing: 5
        anchors.fill: parent

        Text {
            text: " Interaction"
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            color: "lightgrey"
        }
        ToolSeparator {
            anchors.verticalCenter: parent.verticalCenter
        }
        Row {
            id: interactorPositioner
            anchors.verticalCenter: parent.verticalCenter
            spacing: -1
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
                        {
                            interactorButton.checked = false;
                        }
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
                var tmpIncubator
                for(var key in interactorComponentMap)
                    if(interactorComponentMap.hasOwnProperty(key)) {
                        var incubator = interactorButtonComponent.incubateObject(interactorPositioner, {interactorName: key, interactorComponent: interactorComponentMap[key]});
                        tmpIncubator = incubator
                        incubator.forceCompletion();
                    }
            }
        }

        Row {
            id: simulationControlTools
            spacing: -1
            anchors.verticalCenter: parent.verticalCenter

            ToolButton {
                id: animateButton
                width: 22
                iconSource: animateButton.checked ? "qrc:/icon/pause.png" : "qrc:/icon/play.png"
                ToolTip {
                    text: animateButton.checked ? "Stop" : "Animate"
                    description: "Starts / Stop the animation loop"

                }
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
                width: 22
                checkable: false
                iconSource: "qrc:/icon/step.png"
                ToolTip {
                    text: "Step"
                    description: "Performs a single simulation step"
                }

                onClicked: {
                    if(sofaScene)
                        sofaScene.step();
                }
            }

            ToolButton {
                id: resetButton
                width: 22
                checkable: false

                iconSource: "qrc:/icon/replay.png"
                ToolTip {
                    text: "Reset"
                    description: "Reloads the simulation from the scene files"
                }

                onClicked: {
                    if(sofaScene)
                        sofaScene.reload();
                }
            }
            ToolSeparator {
                anchors.verticalCenter: parent.verticalCenter
            }

            Row {
                anchors.verticalCenter: parent.verticalCenter

                Label {
                    color: "black"
                    text: "DT (s) "
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
            anchors.verticalCenter: parent.verticalCenter
            spacing: -1

            ToolButton {
                id: screenshotButton
                iconSource: "qrc:/icon/screenshot.png"
                height: root.height - 5
                width: 22

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

                ToolTip {
                    text: "Save screenshot"
                    description: "Captures a screenshot and saves it on the filesystem"
                }
            }

            ToolButton {
                id: movieButton
                iconSource: "qrc:/icon/movieRecording.png"
                width: 22

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

                ToolTip {
                    text: "Video Capture"
                    description: "Records a video of the simulation (until next press on this button)"

                }
            }
        }
    }
    background: Rectangle {
        color: hovered ? "#757575" : "#686868"
        border.color: "black"

        GBRect {
            anchors.top: parent.top
            anchors.topMargin: 1
            implicitHeight: parent.implicitHeight - 1
            implicitWidth: parent.implicitWidth + 2
            borderWidth: 1
            borderGradient: Gradient {
                GradientStop { position: 0.0; color: "#7a7a7a" }
                GradientStop { position: 1.0; color: "#5c5c5c" }
            }
            color: "transparent"
        }
    }
}

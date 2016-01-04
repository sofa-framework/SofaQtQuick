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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import SofaApplication 1.0

ToolBar {
    id: root
    implicitHeight: 25

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
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        spacing: 5

        Text {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            text: "Interaction"
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            color: "darkblue"
        }

        Row {
            id: interactorPositioner
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Component {
                id: interactorButtonComponent

                ToolButton {
                    id: interactorButton
                    property string interactorName
                    property Component interactorComponent

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
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            spacing: 5

            ToolButton {
                id: animateButton
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                iconSource: animateButton.checked ? "qrc:/icon/stopButton.png" : "qrc:/icon/playButton.png"
                tooltip: animateButton.checked ? "Stop" : "Animate"
                checkable: true
                checked: false
                onCheckedChanged: if(sofaScene) sofaScene.play = animateButton.checked

                Connections {
                    target: sofaScene
                    ignoreUnknownSignals: true
                    onPlayChanged: {
                        animateButton.checked = sofaScene.play;
                    }
                }
            }

            ToolButton {
                id: stepButton
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                iconSource: "qrc:/icon/stepButton.png"
                tooltip: "Step"

                onClicked: {
                    if(sofaScene)
                        sofaScene.step();
                }
            }

            ToolButton {
                id: resetButton
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                iconSource: "qrc:/icon/resetButton.png"
                tooltip: "Reset the sofa scene"

                onClicked: {
                    if(sofaScene)
                        sofaScene.reset();
                }
            }
        }

        Row {
            id: captureLayout
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            spacing: 5

            ToolButton {
                id: screenshotButton
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
                        SofaApplication.takeScreenshot(path);
                    }
                }

                ToolTip {
                    anchors.fill: parent
                    description: "Save screenshot"
                }
            }

            ToolButton {
                id: movieButton
                text: "Movie"
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
                    anchors.fill: parent
                    description: "Save video"
                }
            }
        }
    }
}

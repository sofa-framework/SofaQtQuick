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
//        IconComboBox {
//            id: iteractors
//            anchors.verticalCenter: parent.verticalCenter
//            model: ListModel {
//                id: listmodel
//                ListElement {
//                    name: "Camera Mode"
//                    image: "qrc:/icon/ICON_CAMERA_MODIFIER.png"
//                    interactor: "CameraMode"
//                }
////                ListElement {
////                    name: "Object Mode"
////                    image: "qrc:/icon/ICON_OBJECT_MODE.png"
////                    interactor: "ObjectMode"
////                }
////                ListElement {
////                    name: "Edit Mode"
////                    image: "qrc:/icon/ICON_EDIT_MODE.png"
////                    interactor: "EditMode"
////                }
//            }
//            onCurrentIndexChanged: {
//                if (currentIndex < 0)
//                    return
//                if (SofaApplication.interactorComponentMap.length == 0)
//                    SofaApplication.interactorFolderListModel.update()
//                SofaApplication.interactorComponent = SofaApplication.interactorComponentMap[listmodel.get(currentIndex).interactor]
//            }
//        }

        Row {
            id: simulationControlTools
            spacing: 0
            anchors.verticalCenter: parent.verticalCenter

            ToolButton {
                id: animateButton
                width: 22
                iconSource: animateButton.playing ? "qrc:/icon/pause.png" : "qrc:/icon/play.png"
                iconScale: 1.05
                ToolTip {
                    text: animateButton.playing ? "Stop" : "Animate"
                    description: "Starts / Stop the animation loop"

                }
                checkable: false
                property bool playing: sofaScene.animate
                onClicked: {
                    if(sofaScene) sofaScene.animate = !sofaScene.animate
                }

//                Connections {
//                    target: sofaScene
//                    ignoreUnknownSignals: true
//                    onAnimateChanged: {
//                        animateButton.checked = sofaScene.animate;
//                    }
//                }
            }

            ToolButton {
                id: stepButton
                width: 22
                checkable: false
                iconSource: "qrc:/icon/step.png"
                iconScale: 1.4
                ToolTip {
                    text: "Step"
                    description: "Performs a single simulation step"
                }

                onClicked: {
                    if(sofaScene)
                        sofaScene.step();
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
                iconScale: 0.2
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
                iconScale: 0.2
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

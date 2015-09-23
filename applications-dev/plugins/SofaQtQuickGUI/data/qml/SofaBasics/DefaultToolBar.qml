import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import SceneComponent 1.0
import "qrc:/SofaCommon/SofaCommonScript.js" as SofaCommonScript
import SofaApplication 1.0

ToolBar {
    id: root
    implicitHeight: 25

    property var scene

    Component.onCompleted: SofaApplication.toolBar = root

    enabled: scene ? scene.ready : false

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
                    if(interactorComponentMap.hasOwnProperty(key))
                        SofaCommonScript.InstanciateComponent(interactorButtonComponent, interactorPositioner, {interactorName: key, interactorComponent: interactorComponentMap[key]});
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
                onCheckedChanged: if(scene) scene.play = animateButton.checked

                Connections {
                    target: scene
                    ignoreUnknownSignals: true
                    onPlayChanged: {
                        animateButton.checked = scene.play;
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
                    if(scene)
                        scene.step();
                }
            }

            ToolButton {
                id: resetButton
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                iconSource: "qrc:/icon/resetButton.png"
                tooltip: "Reset the scene"

                onClicked: {
                    if(scene)
                        scene.reset();
                }
            }
        }

        Row {
            id: saveScreenshotTools
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            spacing: 5

            ToolButton {
                id: saveScreenshotButton
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                text: "Save Screenshot"
                tooltip: "Save screenshot"

                onClicked: {
                    if(scene)
                        scene.takeScreenshot();
                        saveSofaSceneScreenshotDialog.open();
                }
            }
        }
    }

/*
    TabView {
        Tab {
            title: "Interaction"

            Row {

            }
        }

        Tab {
            title: "Misc"

            Row {

                ToolButton {
                    text: "A"
                }

                ToolButton {
                    text: "B"
                }

                ToolButton {
                    text: "C"
                }
            }
        }

        style: TabViewStyle {
            frameOverlap: 0
            tabOverlap: -5

            tab: Rectangle {
                implicitWidth: Math.max(text.implicitWidth + 4, 80)
                implicitHeight: 20

                radius: 5
                gradient: Gradient {
                    GradientStop {color: "#EEE"; position: 0.0}
                    GradientStop {color: "#DDD"; position: 0.5}
                    GradientStop {color: styleData.selected ? "#BBB" : "#CCC" ; position: 1.0}
                }

                Text {
                    id: text
                    anchors.centerIn: parent
                    text: styleData.title
                    color: styleData.selected ? "black" : "grey"
                    font.bold: styleData.selected
                }
            }

            frame: null
        }
    }
*/
}

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

import SofaBasics 1.0

GroupBox {
    id: root
    property string clipPlaneComponentPath: "/clipPlane"
    property double distanceMin: -10
    property double distanceMax: 10
    property double distanceStepSize: 0.1
    property var normal: [1,0,0]
    property double distance: 0
    
    onNormalChanged: {
        sofaScene.setDataValue("@"+clipPlaneComponentPath+".normal", [[normal[0], normal[1], normal[2]]]);
        updatePosition();
    }
    
    onDistanceChanged: updatePosition()

    function updatePosition() {
        sofaScene.setDataValue("@"+clipPlaneComponentPath+".position", [[distance*normal[0], distance*normal[1], distance*normal[2]]])
    }
    
    title: "Clipping Plane"
    checkable: true
    checked: false
    
    onCheckedChanged : sofaScene.setDataValue("@"+clipPlaneComponentPath+".active", checked)
    
    GridLayout {
        Layout.fillWidth: true
        columns: 2
        Label {
            Layout.alignment: Qt.AlignRight
            text: "Normal:"
        }
        RowLayout {
            spacing: 20
            ExclusiveGroup {
                id: normalSelection
            }
            RowLayout {
                Button {
                    checkable: true
                    checked: true
                    exclusiveGroup: normalSelection
                    text: "x"
                    implicitWidth: 20
                    onClicked: normal = [1,0,0]
                }
                Button {
                    checkable: true
                    exclusiveGroup: normalSelection
                    text: "-x"
                    implicitWidth: 20
                    onClicked: normal = [-1,0,0]
                }
            }
            RowLayout {
                Button {
                    checkable: true
                    exclusiveGroup: normalSelection
                    text: "y"
                    implicitWidth: 20
                    onClicked: normal = [0,1,0]
                }
                Button {
                    checkable: true
                    exclusiveGroup: normalSelection
                    text: "-y"
                    implicitWidth: 20
                    onClicked: normal = [0,-1,0]
                }
            }
            RowLayout {
                Button {
                    checkable: true
                    exclusiveGroup: normalSelection
                    text: "z"
                    implicitWidth: 20
                    onClicked: normal = [0,0,1]
                }
                Button {
                    checkable: true
                    exclusiveGroup: normalSelection
                    text: "-z"
                    implicitWidth: 20
                    onClicked: normal = [0,0,-1]
                }
            }
        }
        Label {
            Layout.alignment: Qt.AlignRight
            text: "Distance:"
        }
        RowLayout {
            Layout.fillWidth: true
            TextField {
                implicitWidth: 50
                text: root.distanceMin
                validator: DoubleValidator { }
                onAccepted: root.distanceMin = parseFloat(text)
            }
            Slider {
                id: distanceSlider
                Layout.fillWidth: true
                minimumValue: root.distanceMin
                maximumValue: root.distanceMax
                stepSize: root.distanceStepSize
                
                onValueChanged: {
                    root.distance = value;
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    onWheel: {
                        if (wheel.angleDelta.x>0 || wheel.angleDelta.y>0)
                            distanceSlider.value += distanceSlider.stepSize;
                        else
                            distanceSlider.value -= distanceSlider.stepSize;
                    }
                }
            }
            TextField {
                implicitWidth: 50
                text: root.distanceMax
                validator: DoubleValidator { }
                maximumLength: 5
                onAccepted: root.distanceMax = parseFloat(text)
            }
        }
        
    }
    
}

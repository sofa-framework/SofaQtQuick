import QtQuick 2.5
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0

GroupBox {
    id: root
    /// path to the ClipPlane component to control
    property string clipPlaneComponentPath: "/clipPlane"
    /// min/max value for the distance slider
    property double distanceMinMax: 10
    /// precision for distance
    property int distanceNbDecimals: 1
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
                id: distanceTextField
                implicitWidth: 50
                text: root.distance
                validator: DoubleValidator { }
                onAccepted: root.distance = parseFloat(text)
                Connections {
                    target: root
                    onDistanceChanged : {
                        distanceTextField.text = distance.toFixed(root.distanceNbDecimals);
                    }
                }
            }
            Slider {
                id: distanceSlider
                Layout.fillWidth: true
                minimumValue: -1.*root.distanceMinMax
                maximumValue: root.distanceMinMax
                stepSize: Math.pow(10, -1*root.distanceNbDecimals)
                onValueChanged: {
                    root.distance = value;
                }
                Connections {
                    target: root
                    onDistanceChanged : distanceSlider.value = distance
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
        }
        
    }
    
}

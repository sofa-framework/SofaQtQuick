import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

import SofaBasics 1.0

GroupBox {
    id: root
    property string clipPlaneComponentPath: "/clipPlane"
    property double distanceMin: -10
    property double distanceMax: 10
    property double distanceStepSize: 0.1
    
    function setNormal() {
        sofaScene.setDataValue("@"+clipPlaneComponentPath+".normal", [[nx.value, ny.value, nz.value]])
    }
    
    function setDistance() {
        sofaScene.setDataValue("@"+clipPlaneComponentPath+".position", [[nx.value*distance.value, ny.value*distance.value, nz.value*distance.value]])
    }
    
    title: "Clipping Plane"
    checkable: true
    checked: false
    
    onCheckedChanged : sofaScene.setDataValue("@"+clipPlaneComponentPath+".active", checked)
    
    GridLayout {
        columns: 2
        Label {
            Layout.alignment: Qt.AlignRight
            text: "Normal:"
        }
        RowLayout {
            id: normalLayout
            Layout.fillWidth: true
            Button {
                text: "x"
                implicitWidth: 20
                onClicked: {
                    nx.value=1.;
                    ny.value=0.;
                    nz.value=0.;
                }
            }
            Button {
                text: "y"
                implicitWidth: 20
                onClicked: {
                    nx.value=0.;
                    ny.value=1.;
                    nz.value=0.;
                }
            }
            Button {
                text: "z"
                implicitWidth: 20
                onClicked: {
                    nx.value=0.;
                    ny.value=0.;
                    nz.value=1.;
                }
            }
            Label {
                Layout.fillWidth: true
                text: ":"
            }
            SpinBox {
                id: nx
                value: 1
                decimals: 1
                maximumValue: 1
                minimumValue: -1
                stepSize: 0.1
                onValueChanged: setNormal()
            }
            SpinBox {
                id: ny
                value: 0
                decimals: 1
                maximumValue: 1
                minimumValue: -1
                stepSize: 0.1
                onValueChanged: setNormal()
            }
            SpinBox {
                id: nz
                value: 0
                decimals: 1
                maximumValue: 1
                minimumValue: -1
                stepSize: 0.1
                onValueChanged: setNormal()
            }
        }
        Label {
            Layout.alignment: Qt.AlignRight
            text: "Distance:"
        }
        RowLayout {
            Layout.fillWidth: true
            width: normalLayout.width
            Slider {
                id: distanceSlider
                Layout.fillWidth: true
                minimumValue: root.distanceMin
                maximumValue: root.distanceMax
                stepSize: root.distanceStepSize
                
                onValueChanged: {
                    distance.value = value;
                }
            }
            SpinBox {
                id: distance
                value: 0
                decimals: 1
                minimumValue: root.distanceMin
                maximumValue: root.distanceMax
                stepSize: root.distanceStepSize
                onValueChanged: {
                    distanceSlider.value = value;
                    setDistance();
                }
            }
        }
        
    }
    
}
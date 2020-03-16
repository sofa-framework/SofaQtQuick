import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0

Slider {
    id: control
    value: 0.5
    property string displayText: value
    implicitHeight: 20
    implicitWidth: 100
    background: ControlsBackground {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 50
        implicitHeight: 20
        borderColor: control.borderColor === undefined ? control.readOnly ? "#393939" : "#505050" : control.borderColor
        controlType: controlTypes["ToggleButton"]

        width: control.availableWidth

        ControlsBackground {
            width: control.visualPosition * parent.width
            height: parent.height
            borderColor: control.borderColor === undefined ? control.readOnly ? "#393939" : "#505050" : control.borderColor
            controlType: controlTypes["ToggleButton"]
            currentGradients: currentcontrolType.pressed
            z: 5
        }
        Text {
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            text: value
        }
    }

    handle: ControlsBackground {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 12
        implicitHeight: 24
        radius: 4
    }
}

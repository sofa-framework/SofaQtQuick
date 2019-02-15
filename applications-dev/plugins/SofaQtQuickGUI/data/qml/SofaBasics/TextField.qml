import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0

TextField {
    property alias cornerPositions: backgroundID.cornerPositions
    property alias position: backgroundID.position

    id: control
    placeholderText: qsTr("None")
    color: readOnly ? "#464646" : "black"
    leftPadding: 7
    hoverEnabled: true

    background: ControlsBackground {
        id: backgroundID
        implicitWidth: 150
        implicitHeight: 20

        borderColor: control.readOnly ? "#393939" : "#505050";
        controlType: controlTypes["InputField"]
    }

    onActiveFocusChanged: {
        backgroundID.setControlState(control.enabled && !control.readOnly, control.hovered, control.activeFocus)
    }
    onHoveredChanged: {
        backgroundID.setControlState(control.enabled && !control.readOnly, control.hovered, control.activeFocus)
    }
    Component.onCompleted: {
        backgroundID.setControlState(control.enabled && !control.readOnly, control.hovered, control.activeFocus)
    }


//    DropShadow {
//        z: -1
//        anchors.fill: backgroundRect
//        horizontalOffset: 0
//        verticalOffset: -1
//        radius: 4.0
//        samples: 17
//        color: "#50000000"
//        source: backgroundRect
//    }
}

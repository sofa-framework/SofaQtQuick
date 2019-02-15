import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0

Button {
    id: control
    hoverEnabled: true
    enabled: true

//    text: "my button"

    property alias cornerPositions: backgroundID.cornerPositions

    onDownChanged: {
        backgroundID.setControlState(control.enabled, control.hovered, control.down)
    }
    onHoveredChanged: {
        backgroundID.setControlState(control.enabled, control.hovered, control.down)
    }
    Component.onCompleted: {
        backgroundID.setControlState(control.enabled, control.hovered, control.down)
    }

    leftPadding: 7
    rightPadding: leftPadding
    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.4
        color: control.down ? "#DDDDDD" : "black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    property alias position: backgroundID.position
    background: ControlsBackground {
        id: backgroundID
        controlType: controlTypes["Button"]
        borderColor: enabled ? "#595959" : "#898989"
    }
//    DropShadow {
//        anchors.fill: backgroundID
//        horizontalOffset: 0
//        verticalOffset: 1
//        radius: 4.0
//        samples: 17
//        color: "#10000000"
//        source: backgroundID
//    }
}

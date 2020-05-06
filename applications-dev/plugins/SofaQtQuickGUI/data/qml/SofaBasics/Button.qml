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

    leftPadding: 7
    rightPadding: leftPadding
    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.4
        color: checkable ? (control.checked ? "black" : "#DDDDDD") : (control.down ? "#DDDDDD" : "black")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    activeFocusOnTab: true
    onActiveFocusChanged: {
        backgroundID.setControlState(control.enabled, control.focus, (checkable ? control.checked : control.down))
    }

    property alias position: backgroundID.position
    background: ControlsBackground {
        id: backgroundID
        controlType: checkable ? controlTypes["ToggleButton"] : controlTypes["Button"]
        borderColor: enabled ? "#595959" : "#898989"
    }
    onDownChanged: {
        backgroundID.setControlState(control.enabled, control.hovered, (checkable ? control.checked : control.down))
    }
    onCheckedChanged: {
        backgroundID.setControlState(control.enabled, control.hovered, (checkable ? control.checked : control.down))
    }
    onHoveredChanged: {
        backgroundID.setControlState(control.enabled, control.hovered, (checkable ? control.checked : control.down))
    }
    Component.onCompleted: {
        backgroundID.setControlState(control.enabled, control.hovered, (checkable ? control.checked : control.down))
    }
}

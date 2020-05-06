import QtQuick 2.0
import QtQuick.Controls 2.4
import SofaColorScheme 1.0

ToolButton {
    id: control
    enabled: true
    hoverEnabled: true
    property string iconSource: ""
    property var iconScale: 1

    anchors.verticalCenter: parent.verticalCenter
    implicitHeight: 20
    width: 20
    text: " "

    leftPadding: 7
    rightPadding: leftPadding

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.4
        color: control.checkable ? (control.checked ? "black" : "#DDDDDD") : (control.down ? "#DDDDDD" : "black")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    Rectangle {
        anchors.centerIn: parent
        implicitWidth: 16
        implicitHeight: 16
        width: 16
        height: 16
        color: "transparent"
        Image {
            id: image
            anchors.centerIn: parent
            source: iconSource
            scale: control.iconScale
        }
    }


    property alias position: backgroundID.position
    property alias cornerPositions: backgroundID.cornerPositions
    background: ControlsBackground {
        id: backgroundID
        position: cornerPositions["Middle"]
        controlType: control.checkable ? controlTypes["ToggleButton"] : controlTypes["Button"]
        borderColor: control.enabled ? "#595959" : "#898989"
    }
    onDownChanged: {
        backgroundID.setControlState(control.enabled, control.hovered, (checkable ? control.checked : control.down))
    }
    onEnabledChanged: {
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

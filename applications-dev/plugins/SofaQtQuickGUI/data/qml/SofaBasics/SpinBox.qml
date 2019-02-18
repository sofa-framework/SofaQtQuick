import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0

SpinBox {
    property alias cornerPositions: backgroundID.cornerPositions
    property alias position: backgroundID.position

    id: control
    value: 50
    editable: false
    leftPadding: 20
    rightPadding: 20
    hoverEnabled: true
    height: 20
    implicitHeight: 20

    onActiveFocusChanged: {
        backgroundID.setControlState(control.enabled && control.editable, control.hovered, control.activeFocus)
    }
    onHoveredChanged: {
        backgroundID.setControlState(control.enabled && control.editable, control.hovered, control.activeFocus)
    }
    Component.onCompleted: {
        backgroundID.setControlState(control.enabled && control.editable, control.hovered, control.activeFocus)
    }

    contentItem: TextInput {
        z: 2
        anchors.fill: backgroundID
        text: control.textFromValue(control.value, control.locale)
        font: control.font
        color: control.editable ? "black" : "#464646"
        //        selectionColor: "#21be2b"
        //        selectedTextColor: "#ffffff"
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly
    }

    up.indicator: Image {
        id: rightup
        x: control.mirrored ? 7 : parent.width - width - 7
        y: parent.height / 4
        width: 9
        height: width
        source: "qrc:icon/spinboxRight.png"
    }

    down.indicator: Image {
        id: leftdown
        x: control.mirrored ? parent.width - width - 7 : 7
        y: parent.height / 4
        width: 9
        height: width
        source: "qrc:icon/spinboxLeft.png"
    }

    background: ControlsBackground {
        id: backgroundID
        implicitWidth: 40
        implicitHeight: 20

        borderColor: control.readOnly ? "#393939" : "#505050";
        controlType: controlTypes["InputField"]
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

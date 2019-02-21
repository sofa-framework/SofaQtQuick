import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0

SpinBox {
    property alias cornerPositions: backgroundID.cornerPositions
    property alias position: backgroundID.position

    property string prefix: ""
    property string suffix: ""

    id: control
    value: 50
    editable: true
    hoverEnabled: true
    height: 20
    implicitHeight: 20
    implicitWidth: inputText.implicitWidth + up.indicator.width + down.indicator.width

    onActiveFocusChanged: {
        backgroundID.setControlState(control.enabled && control.editable, control.hovered, control.activeFocus)
    }
    onHoveredChanged: {
        backgroundID.setControlState(control.enabled && control.editable, control.hovered, control.activeFocus)
    }
    Component.onCompleted: {
        backgroundID.setControlState(control.enabled && control.editable, control.hovered, control.activeFocus)
    }

    function formatText(value, locale, prefix, suffix) {
        var str = ""
        if (prefix !== "")
            str += qsTr(prefix)
        str += qsTr("%1").arg(value)
        if (suffix !== "")
            str += qsTr(suffix)
        return str
    }

    contentItem: Rectangle {
        anchors.fill: parent
        color: "transparent"
        TextInput {
            id: inputText
            z: 2
            anchors.centerIn: parent
            text: formatText(value, locale, prefix, suffix)
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
    }

    up.indicator: Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        width: 20
        height: parent.height
        color: "transparent"
        Image {
            id: rightup
            x: control.mirrored ? 7 : parent.width - width - 7
            y: parent.height / 4
            width: 9
            height: width
            source: "qrc:icon/spinboxRight.png"
        }
    }

    down.indicator: Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        width: 20
        height: parent.height
        color: "transparent"
        Image {
            id: leftdown
            x: control.mirrored ? parent.width - width - 7 : 7
            y: parent.height / 4
            width: 9
            height: width
            source: "qrc:icon/spinboxLeft.png"
        }
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

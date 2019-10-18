import QtQuick 2.4
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0

CheckBox {
    property alias cornerPositions: backgroundID.cornerPositions

    id: control
    implicitHeight: 16
    implicitWidth: 16
    checkable: true
    hoverEnabled: true

    activeFocusOnTab: true
    onActiveFocusChanged: backgroundID.setControlState((enabled && checkable), activeFocus, checked)

    background: ControlsBackground {
        id: backgroundID
        controlType: controlTypes["CheckBox"]
        borderColor: control.checkable ? "#393939" : "#696969";
    }

    onHoveredChanged: {
        backgroundID.setControlState((enabled && checkable), hovered, checked)
    }
    Component.onCompleted: {
        backgroundID.setControlState((enabled && checkable), hovered, checked)
    }

    indicator: Image {
        visible: control.checked ? true : false
        width: 18
        height: width
        x : 1
        y : -3
        source: "qrc:icon/check.png"
    }

//    DropShadow {
//        z: -1
//        anchors.fill: backgroundID
//        horizontalOffset: 0
//        verticalOffset: -1
//        radius: 4.0
//        samples: 17
//        color: "#50000000"
//        source: backgroundID
//    }

}


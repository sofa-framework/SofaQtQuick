import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

SpinBox {
    id: control
    value: 50
    editable: true
    leftPadding: 20
    property Gradient rwGradient: Gradient {
        GradientStop { position: 0.0; color: "#D0D0D0" }
        GradientStop { position: 1.0; color: "#E0E0E0" }
    }

    property Gradient roGradient: Gradient {
        GradientStop { position: 0.0; color: "#898989" }
        GradientStop { position: 1.0; color: "#929292" }
    }
    property Gradient rwHoverGradient: Gradient {
        GradientStop { position: 0.0; color: "#E0E0E0" }
        GradientStop { position: 1.0; color: "#F0F0F0" }
    }

    property Gradient roHoverGradient: Gradient {
        GradientStop { position: 0.0; color: "#909090" }
        GradientStop { position: 1.0; color: "#999999" }
    }

    hoverEnabled: true
    onHoveredChanged: {
        if (control.hovered)
            backgroundRect.gradient = !control.editable ? roHoverGradient : rwHoverGradient
        else
            backgroundRect.gradient = !control.editable ? roGradient : rwGradient
    }

    contentItem: TextInput {
        z: 2
        anchors.fill: backgroundRect
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

    background: Rectangle {
        id: backgroundRect
        radius: 8
        implicitWidth: 140
        implicitHeight: 20
        border.color: control.readOnly ? "#393939" : "#505050";
        gradient: control.editable ?  rwGradient : roGradient;
    }
    DropShadow {
        z: -1
        anchors.fill: backgroundRect
        horizontalOffset: 0
        verticalOffset: -1
        radius: 4.0
        samples: 17
        color: "#50000000"
        source: backgroundRect
    }

}

import QtQuick 2.4
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

CheckBox {
    id: control
    implicitHeight: 16
    implicitWidth: 16
    checkable: true
    hoverEnabled: true

    property Gradient enabledGradient: Gradient {
        GradientStop { position: 0.0; color: "#636363" }
        GradientStop { position: 1.0; color: "#373737" }
    }

    property Gradient disabledGradient: Gradient {
        GradientStop { position: 0.0; color: "#797979" }
        GradientStop { position: 1.0; color: "#999999" }
    }
    property Gradient enabledHoverGradient: Gradient {
        GradientStop { position: 0.0; color: "#737373" }
        GradientStop { position: 1.0; color: "#474747" }
    }

    property Gradient disabledHoverGradient: Gradient {
        GradientStop { position: 0.0; color: "#909090" }
        GradientStop { position: 1.0; color: "#999999" }
    }

    property Gradient enabledGradientReversed: Gradient {
        GradientStop { position: 1.0; color: "#636363" }
        GradientStop { position: 0.0; color: "#373737" }
    }

    property Gradient disabledGradientReversed: Gradient {
        GradientStop { position: 1.0; color: "#797979" }
        GradientStop { position: 0.0; color: "#999999" }
    }
    property Gradient enabledHoverGradientReversed: Gradient {
        GradientStop { position: 1.0; color: "#737373" }
        GradientStop { position: 0.0; color: "#474747" }
    }

    property Gradient disabledHoverGradientReversed: Gradient {
        GradientStop { position: 1.0; color: "#909090" }
        GradientStop { position: 0.0; color: "#999999" }
    }

    background: Rectangle {
        id: backgroundRect
        radius: 4
        border.color: control.checkable ? "#393939" : "#808080";
        gradient: control.checkable ?  enabledGradient : disabledGradient;
    }

    onHoveredChanged: {
        if (control.hovered) {
            if (control.checkable) {
                if (!control.checked) {
                    backgroundRect.gradient = enabledHoverGradient
                } else {
                    backgroundRect.gradient = enabledHoverGradientReversed
                }
            } else {
                if (!control.checked) {
                    backgroundRect.gradient = disabledHoverGradient
                } else {
                    backgroundRect.gradient = disabledHoverGradientReversed
                }
            }
        } else {
            if (control.checkable) {
                if (!control.checked) {
                    backgroundRect.gradient = enabledGradient
                } else {
                    backgroundRect.gradient = enabledGradientReversed
                }
            } else {
                if (!control.checked) {
                    backgroundRect.gradient = disabledGradient
                } else {
                    backgroundRect.gradient = disabledGradientReversed
                }
            }
        }
    }

    indicator: Image {
        visible: control.checked ? true : false
        width: 18
        height: width
        x : 1
        y : -3
        source: "qrc:icon/check.png"
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


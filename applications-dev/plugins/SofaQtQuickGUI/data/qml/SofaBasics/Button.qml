import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

Button {
    id: control
    hoverEnabled: true
    enabled: true
    property Gradient enabledGradient: Gradient {
        GradientStop { position: 0.0; color: "#c6c6c6" }
        GradientStop { position: 1.0; color: "#aaaaaa" }
    }
    property Gradient disabledGradient: Gradient {
        GradientStop { position: 0.0; color: "#797979" }
        GradientStop { position: 1.0; color: "#999999" }
    }
    property Gradient enabledHoverGradient: Gradient {
        GradientStop { position: 0.0; color: "#d6d6d6" }
        GradientStop { position: 1.0; color: "#bababa" }
    }

    property Gradient disabledHoverGradient: Gradient {
        GradientStop { position: 0.0; color: "#909090" }
        GradientStop { position: 1.0; color: "#999999" }
    }

    property Gradient enabledGradientDown: Gradient {
        GradientStop { position: 1.0; color: "#636363" }
        GradientStop { position: 0.0; color: "#373737" }
    }

    property Gradient disabledGradientDown: Gradient {
        GradientStop { position: 1.0; color: "#797979" }
        GradientStop { position: 0.0; color: "#999999" }
    }

    onDownChanged: {
        if (control.down)
            backgroundRect.gradient = control.enabled ? enabledGradientDown : disabledGradientDown
        else
            backgroundRect.gradient = control.enabled ? enabledGradient : disabledGradient
    }
    onHoveredChanged: {
        if (control.hovered) {
            backgroundRect.gradient = control.enabled ? enabledHoverGradient : disabledHoverGradient
        } else {
            backgroundRect.gradient = control.enabled ? enabledGradient : disabledGradient
        }
    }

    leftPadding: 7
    rightPadding: leftPadding
    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.down ? "#DDDDDD" : "black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        id: backgroundRect
        implicitWidth: 20
        implicitHeight: 20
        opacity: enabled ? 1 : 0.3
        border.color: "#393939"
        border.width: 1
        radius: 4
        gradient: control.enabled ?  enabledGradient : disabledGradient;
    }
    DropShadow {
        anchors.fill: backgroundRect
        horizontalOffset: 0
        verticalOffset: 1
        radius: 4.0
        samples: 17
        color: "#10000000"
        source: backgroundRect
    }
}

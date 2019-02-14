import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

TextField {
    id: control
    placeholderText: qsTr("None")
    color: readOnly ? "#464646" : "black"
    leftPadding: 7
    hoverEnabled: true

    background: Rectangle {
        id: backgroundRect
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

        radius: 8
        implicitWidth: 150
        implicitHeight: 20

        border.color: control.readOnly ? "#393939" : "#505050";
        gradient: control.readOnly ?  roGradient : rwGradient;
    }
    onHoveredChanged: {
        if (control.hovered)
            backgroundRect.gradient = control.readOnly ? backgroundRect.roHoverGradient : backgroundRect.rwHoverGradient
        else
            backgroundRect.gradient = control.readOnly ? backgroundRect.roGradient : backgroundRect.rwGradient
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

import QtQuick 2.6
import QtQuick.Controls 2.4

ComboBox {
    id: control

    implicitHeight: 20
    implicitWidth: 100

//    model: ["Cats", "Dogs", "Bunnies"]
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

    property Gradient enabledGradientDown: Gradient {
        GradientStop { position: 1.0; color: "#636363" }
        GradientStop { position: 0.0; color: "#373737" }
    }

    property Gradient disabledGradientDown: Gradient {
        GradientStop { position: 1.0; color: "#797979" }
        GradientStop { position: 0.0; color: "#999999" }
    }
    property Gradient enabledHoverGradientDown: Gradient {
        GradientStop { position: 1.0; color: "#737373" }
        GradientStop { position: 0.0; color: "#474747" }
    }

    property Gradient disabledHoverGradientDown: Gradient {
        GradientStop { position: 1.0; color: "#909090" }
        GradientStop { position: 0.0; color: "#999999" }
    }

    onDownChanged: {
        if (control.hovered) {
            if (control.down)
                backgroundRect.gradient = control.enabled ? enabledHoverGradientDown : disabledHoverGradientDown
            else
                backgroundRect.gradient = control.enabled ? enabledHoverGradient : disabledHoverGradient
        } else {
            if (control.down)
                backgroundRect.gradient = control.enabled ? enabledGradientDown : disabledGradientDown
            else
                backgroundRect.gradient = control.enabled ? enabledGradient : disabledGradient
        }
    }
    onHoveredChanged: {
        if (control.hovered) {
            if (control.down)
                backgroundRect.gradient = control.enabled ? enabledHoverGradientDown : disabledHoverGradientDown
            else
                backgroundRect.gradient = control.enabled ? enabledHoverGradient : disabledHoverGradient
        } else {
            if (control.down)
                backgroundRect.gradient = control.enabled ? enabledGradientDown : disabledGradientDown
            else
                backgroundRect.gradient = control.enabled ? enabledGradient : disabledGradient
        }
    }

    /// TODO @bmarques: Fix modelData bug
//    delegate: ItemDelegate {
//        width: control.width
//        height: 20
//        opacity: 0.4
//        contentItem: Text {
//            text: modelData
//            color: "black"
//            font: control.font
//            elide: Text.ElideRight
//            verticalAlignment: Text.AlignVCenter
//        }
//        highlighted: control.highlightedIndex === index
//    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"
        Connections {
            target: control
            onPressedChanged: canvas.requestPaint()
        }
        opacity: 0.4

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = control.down ? "#AAAAAA" : "#DDDDDD";
            context.fill();
        }
    }

    contentItem: Text {
        leftPadding: 7
        rightPadding: control.indicator.width + control.spacing

        text: control.displayText
        font: control.font
        color: control.down? "#AAAAAA" : "#DDDDDD"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        id: backgroundRect
        implicitWidth: 50
        implicitHeight: 20
        height: 20
        border.color: control.down ? "#17a81a" : "#21be2b"
        border.width: control.visualFocus ? 2 : 1
        radius: 4
        gradient: control.enabled ?  enabledGradient : disabledGradient;
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 0

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            id : popupRect
            border.color: control.enabled ? "#393939" : "#808080";
            radius: 4
            color: "transparent"
        }
    }
}

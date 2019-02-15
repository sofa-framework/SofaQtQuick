import QtQuick 2.6
import QtQuick.Controls 2.4
import SofaColorScheme 1.0

ComboBox {
    property alias cornerPositions: backgroundID.cornerPositions

    id: control
    implicitHeight: 20
    implicitWidth: 100
    enabled: true
//    model: ["Cats", "Dogs", "Bunnies"]
    hoverEnabled: true


    onDownChanged: {
        backgroundID.setControlState(enabled, hovered, down)
    }
    onHoveredChanged: {
        backgroundID.setControlState(enabled, hovered, down)
    }
    Component.onCompleted: {
        backgroundID.setControlState(enabled, hovered, down)
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

    background: ControlsBackground {
        id: backgroundID
        implicitWidth: 50
        implicitHeight: 20
        height: 20
        borderColor: control.enabled ? "#393939" : "#808080";
        controlType: controlTypes["ComboBox"]
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

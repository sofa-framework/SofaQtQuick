import QtQuick 2.6
import QtQuick.Controls 2.4
import SofaColorScheme 1.0

ComboBox {
    property alias cornerPositions: backgroundID.cornerPositions
    property bool sizeToContents: true
    property int modelWidth: 40

    id: control
    implicitHeight: 20
    width: 100
    implicitWidth: (sizeToContents) ? (modelWidth + 2*leftPadding + 2*rightPadding + canvas.width) : width
    enabled: true
    model: ["Cats", "Dogs", "Bunnies"]
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

    delegate: ItemDelegate {
        width: control.width
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
        font.weight: control.currentIndex === index ? Font.DemiBold : Font.Normal
        font.family: control.font.family
        font.pointSize: control.font.pointSize
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
    }

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
        height: 20
        borderColor: control.enabled ? "#393939" : "#808080";
        controlType: controlTypes["ComboBox"]
    }

    popup: Popup {
        y: control.height - 1
        implicitWidth: control.width
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

    TextMetrics {
        id: textMetrics
    }

    function getModelWidth() {
        if (!model)
            return
        textMetrics.font = control.font
        for(var i = 0; i < model.length; i++) {
            if (textRole) {
                textMetrics.text = model.get(i)[textRole]
            }
            else
                textMetrics.text = model[i]
            modelWidth = Math.max(textMetrics.width, modelWidth)
        }
    }

    onModelChanged: {
        getModelWidth()
    }
}

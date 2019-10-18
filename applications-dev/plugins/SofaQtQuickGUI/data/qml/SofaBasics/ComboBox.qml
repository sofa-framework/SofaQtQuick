import QtQuick 2.6
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0
import SofaBasics 1.0 as SB
import SofaApplication 1.0

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

    activeFocusOnTab: true
    onActiveFocusChanged: backgroundID.setControlState(enabled, activeFocus, down)
    onDownChanged: backgroundID.setControlState(enabled, hovered, down)
    onHoveredChanged: backgroundID.setControlState(enabled, hovered, down)
    Component.onCompleted: backgroundID.setControlState(enabled, hovered, down)

    delegate: ItemDelegate {
        id: itemDelegate
        width: control.width
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
        contentItem: Text {
            leftPadding: 5
            text: itemDelegate.text
            font: control.font
            color: itemDelegate.highlighted ? "black" : itemDelegate.hovered ? "lightgrey" : "white"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
        background: Rectangle {

            property Gradient highlightcolor: Gradient {
                GradientStop { position: 0.0; color: "#7aa3e5" }
                GradientStop { position: 1.0; color: "#5680c1" }
            }
            property Gradient nocolor: Gradient {
                GradientStop { position: 0.0; color: SofaApplication.style.transparentBackgroundColor }
                GradientStop { position: 1.0; color: SofaApplication.style.transparentBackgroundColor }
            }

            anchors.fill: parent
            implicitHeight: 20
            gradient: itemDelegate.highlighted ? highlightcolor : nocolor
        }
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

    contentItem: Text { // The text in the top part of the combobox
        leftPadding: 7
        rightPadding: control.indicator.width + control.spacing

        text: control.displayText
        font: control.font
        color: control.down? "#AAAAAA" : "#DDDDDD"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: ControlsBackground { // The background of the top part of the combobox
        id: backgroundID
        height: 20
        borderColor: control.enabled ? "#393939" : "#808080";
        controlType: controlTypes["ComboBox"]
    }

    popup: Popup {
        id: popupId
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            id: backgroundId
            border.color: "transparent"
            color: "transparent"
            Rectangle {
                id: topRect
                anchors.top: backgroundId.top
                implicitWidth: backgroundId.implicitWidth
                implicitHeight: 5
                color: "#70393939"
            }
            Rectangle {
                id: bottomRect
                anchors.fill: backgroundId
                color: SofaApplication.style.transparentBackgroundColor
                radius: 5
                anchors.bottom: backgroundId.bottom
                anchors.bottomMargin: -5
            }
            DropShadow {
                z: -1
                anchors.fill: bottomRect
                horizontalOffset: 0
                verticalOffset: -1
                radius: 4.0
                samples: 17
                color: SofaApplication.style.transparentBackgroundColor
                source: bottomRect
            }
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

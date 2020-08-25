import QtQuick 2.6
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0
import SofaBasics 1.0 as SB
import SofaApplication 1.0

ComboBox {
    id: control

    property alias cornerPositions: backgroundID.cornerPositions
    property bool sizeToContents: true
    property int modelWidth: 40
    implicitHeight: 20

    implicitWidth: (sizeToContents) ? modelWidth + 20 + 70 + canvas.width : width
    enabled: true
    model: ListModel {
        id: listmodel
        ListElement {
            name: "Camera Mode"
            image: "qrc:/icon/ICON_CAMERA_MODIFIER.png"
        }
//        ListElement {
//            name: "Object Mode"
//            image: "qrc:/icon/ICON_OBJECT_MODE.png"
//        }
//        ListElement {
//            name: "Edit Mode"
//            image: "qrc:/icon/ICON_EDIT_MODE.png"
//        }
    }
    hoverEnabled: true

    activeFocusOnTab: true
    onEnabledChanged: backgroundID.setControlState(control.enabled, control.activeFocus, control.down)
    onActiveFocusChanged: backgroundID.setControlState(control.enabled, control.activeFocus, control.down)
    onDownChanged: backgroundID.setControlState(control.enabled, control.hovered, control.down)
    onHoveredChanged: backgroundID.setControlState(control.enabled, control.hovered, control.down)
    Component.onCompleted: backgroundID.setControlState(control.enabled, control.hovered, control.down)

    delegate: ItemDelegate {
        id: itemDelegate
        width: control.width
        contentItem: Row {
            Rectangle {
                width: 7
                height: 7
                color: "transparent"
            }

            Image {
                source: image
                anchors.verticalCenter: parent.verticalCenter
                sourceSize.width: 20
                sourceSize.height: 20
            }
            Text {
                leftPadding: 5
                text: name
                color: itemDelegate.highlighted ? "black" : itemDelegate.hovered ? "lightgrey" : "white"
                elide: Text.ElideRight
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
            }
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
            function onPressedChanged(pressed) { canvas.requestPaint() }
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

    contentItem: Row {
        anchors.centerIn: parent
        Image {
            source: control.model.get(control.currentIndex).image
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            leftPadding: 7
            rightPadding: control.indicator.width + control.spacing

            text: control.model.get(control.currentIndex).name
            font: control.font
            color: control.down? "#AAAAAA" : "#DDDDDD"
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            anchors.verticalCenter: parent.verticalCenter
        }
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
            textMetrics.text = model.get(i).name
            modelWidth = Math.max(textMetrics.width + 20, modelWidth)
        }
    }

    onModelChanged: {
        getModelWidth()
    }
}

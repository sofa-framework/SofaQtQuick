import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import SofaColorScheme 1.0
import SofaApplication 1.0

Item {
    id: control
    property int panelWidth: 200
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    anchors.topMargin: 0
    anchors.bottomMargin: 0
    implicitWidth: panelWidth
    property alias control: loader.sourceComponent
    GBRect {
        z: -1
        id: controlPanel
        //        color: "lightgrey"
        anchors.fill: parent
        //        cornerRadius: 4
        visible: false
        borderWidth: 1
        borderGradient: Gradient {
            GradientStop { position: 0.0; color: "#7a7a7a" }
            GradientStop { position: 1.0; color: "#5c5c5c" }
        }
        color: SofaApplication.style.contentBackgroundColor

        // avoid mouse event propagation through the toolpanel to the sofa viewer
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            onWheel: wheel.accepted = true
        }
        // Copy Blender-style side panels behavior
        MouseArea {
            property int oldMouseX

            id: resizeArea
            anchors.left: parent.left
            anchors.leftMargin: -2
            width: 5
            height: parent.height
            hoverEnabled: true
            onHoveredChanged: {
                cursorShape = Qt.SizeHorCursor
            }

            onPressed: {
                oldMouseX = mouseX
            }

            onPositionChanged: {
                if (pressed) {
                    if(control.implicitWidth < 20) {
                        control.implicitWidth = panelWidth
                        controlPanel.visible = false
                    }
                    else control.implicitWidth = control.implicitWidth + (oldMouseX - mouseX)
                }
            }
        }
        Loader {
            id: loader
            anchors.fill: parent
        }
    }
    ControlsBackground {
        id: controlSwitch
        cornerRadius: 4
        position: cornerPositions["Left"]
        gradient: null
        backgroundColor: "#40000000"
        borderColor: "transparent"
        noGradient: true
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 26
        anchors.rightMargin: 0
        visible: !controlPanel.visible
        width: 16
        height: 12
        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: controlPanel.visible = !controlPanel.visible
        }
    }
    Image {
        source: "qrc:/icon/plus.png"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 28
        anchors.rightMargin: 5
        width: 8
        height: width
        visible: !controlPanel.visible
    }
}

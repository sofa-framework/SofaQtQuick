import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12

MenuItem {
    id: control
    property var style: "normal"
    hoverEnabled: true
    onHoveredChanged: highlighted = hovered
    implicitWidth: parent.implicitWidth
    padding: 6
    spacing: 6
    enabled: true

    icon.width: 20
    icon.height: 20
    icon.color: "transparent"
    contentItem: IconLabel {
        readonly property real arrowPadding: control.subMenu && control.arrow ? control.arrow.width + control.spacing : 0
        readonly property real indicatorPadding: control.checkable && control.indicator ? control.indicator.width + control.spacing : 0
        leftPadding: !control.mirrored ? indicatorPadding : arrowPadding
        rightPadding: control.mirrored ? indicatorPadding : arrowPadding

        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        alignment: Qt.AlignLeft

        icon: control.icon
        text: control.text
        font: control.font
        color: control.enabled ? control.highlighted ? "black" : "white" : "grey"
    }

    indicator: ColorImage {
        x: control.mirrored ? control.width - width - control.rightPadding : control.leftPadding
        y: control.topPadding + (control.availableHeight - height) / 2

        source: control.checkable ? (control.checked ? "qrc:/icon/menuCheckBox.png" : "qrc:/icon/menuCheckBoxChecked.png") : ""
    }
    arrow: Loader {
        Component {
            id: imNormal
            Image {
                x: control.mirrored ? control.leftPadding : control.width - width - control.rightPadding
                y: control.topPadding + (control.availableHeight - height) / 2

                visible: control.subMenu
                mirror: control.mirrored
                source: {
                    return control.subMenu ? (control.highlighted ? "qrc:/icon/menuArrowDown.png" : "qrc:/icon/menuArrow.png") : ""
                }
            }
        }

        Component {
            id: imOverlay
            Image {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: -14
                anchors.bottomMargin: -14
                source: "qrc:/icon/ManipulatorMenuCorner.png"
                scale: 0.35
                opacity: 0.7
                visible: control.subMenu
                mirror: control.mirrored
            }
        }
        sourceComponent: style === "overlay" ? imOverlay : imNormal
    }

    property Gradient highlight: Gradient {
        GradientStop { position: 0.0; color: "#7aa3e5" }
        GradientStop { position: 1.0; color: "#5680c1" }
    }
    property Gradient none: Gradient {
        GradientStop { position: 0.0; color: "transparent" }
        GradientStop { position: 1.0; color: "transparent" }
    }

    Component {
        id: bgOverlay
        Rectangle {
            implicitHeight: 30
            color: control.highlighted ? "#5680c2" : "#525252"
        }
    }
    Component {
        id: bgNormal
        Rectangle {
            x: 1
            y: 1
            gradient: control.highlighted ? highlight : none
        }

    }

    background: Loader {
        sourceComponent: style === "overlay" ? bgOverlay : bgNormal
    }
}

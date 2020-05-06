import QtQuick 2.0
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import SofaBasics 1.0 as SB
import SofaApplication 1.0

Menu {
    id: root
    property var style: "normal"
    delegate: SB.MenuItem {
        style: root.style
    }

    Component {
        id: bgNormal
        Rectangle {
            implicitWidth: 200
            implicitHeight: 20
            color: "transparent"
            Rectangle {
                id: topRect
                anchors.top: parent.top
                implicitWidth: parent.implicitWidth
                implicitHeight: 5
                color: SofaApplication.style.transparentBackgroundColor
            }
            Rectangle {
                id: bottomRect
                anchors.fill: parent
                color: SofaApplication.style.transparentBackgroundColor
                radius: 5
                anchors.bottom: parent.bottom
                anchors.bottomMargin: -5
            }
            DropShadow {
                z: -1
                anchors.fill: bottomRect
                horizontalOffset: 3
                verticalOffset: 3
                radius: 8.0
                samples: 17
                color: "#FF000000"
                source: bottomRect
            }
        }
    }

    Component {
        id: bgOverlay
        Rectangle {
            implicitWidth: 30
            implicitHeight: 30
            Rectangle {
                anchors.fill: parent
                color: "transparent"
            }
        }
    }

    background: Loader {
        sourceComponent: style === "overlay" ? bgOverlay : bgNormal
    }
}


//            padding: 6
//            spacing: 6


//            icon.width: 30
//            icon.height: 30
//            icon.color: "transparent"
//            contentItem: IconLabel {
//                readonly property real arrowPadding: control.subMenu && control.arrow ? control.arrow.width + control.spacing : 0
//                readonly property real indicatorPadding: control.checkable && control.indicator ? control.indicator.width + control.spacing : 0
//                leftPadding: !control.mirrored ? indicatorPadding : arrowPadding
//                rightPadding: control.mirrored ? indicatorPadding : arrowPadding

//                spacing: control.spacing
//                mirrored: control.mirrored
//                display: control.display
//                alignment: Qt.AlignLeft

//                icon: control.icon
//                text: control.text
//                font: control.font
//                color: control.enabled ? control.hovered || control.highlighted ? "#686868" : "#525252" : "#323232"
//            }

//            indicator: ColorImage {
//                x: control.mirrored ? control.width - width - control.rightPadding : control.leftPadding
//                y: control.topPadding + (control.availableHeight - height) / 2

//                source: control.checkable ? (control.checked ? "qrc:/icon/menuCheckBox.png" : "qrc:/icon/menuCheckBoxChecked.png") : ""
//            }
//            arrow: Image {
//                x: control.mirrored ? control.leftPadding : control.width - width - control.rightPadding
//                y: control.topPadding + (control.availableHeight - height) / 2

//                visible: control.subMenu
//                mirror: control.mirrored
//                source: control.subMenu ? (control.highlighted ? "qrc:/icon/menuArrowDown.png" : "qrc:/icon/menuArrow.png") : ""
//            }

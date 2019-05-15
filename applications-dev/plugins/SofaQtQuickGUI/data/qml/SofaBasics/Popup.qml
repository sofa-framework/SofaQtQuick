import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0
import SofaApplication 1.0

Popup {
    id: popup

    background: Rectangle {
        id: background
        implicitWidth: 200
        implicitHeight: 200
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
            horizontalOffset: 0
            verticalOffset: -1
            radius: 4.0
            samples: 17
            color: "#FF000000"
            source: bottomRect
        }
    }
}

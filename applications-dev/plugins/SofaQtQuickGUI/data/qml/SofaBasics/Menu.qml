import QtQuick 2.0
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0
import SofaBasics 1.0 as SB

Menu {

    delegate: SB.MenuItem {}
    background: Rectangle {
        id: backgroundID
        implicitWidth: 200
        implicitHeight: 20
        color: "transparent"
        Rectangle {
            id: topRect
            anchors.top: parent.top
            implicitWidth: parent.implicitWidth
            implicitHeight: 5
            color: "#70000000"
        }
        Rectangle {
            id: bottomRect
            anchors.fill: parent
            color: "#70000000"
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

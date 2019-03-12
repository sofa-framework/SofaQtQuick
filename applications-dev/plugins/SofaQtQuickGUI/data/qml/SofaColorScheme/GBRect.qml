import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {
    id: root
    property Gradient borderGradient
    property bool isHorizontal: false
    property int borderWidth: 0


    Loader {
        id: loader
        active: borderGradient
        anchors.fill: parent
        sourceComponent: border
    }

    Component {
        id: border
        Item {
            Rectangle {
                id: borderFill
                radius: root.radius
                anchors.fill: parent
                LinearGradient {
                    source: parent
                    anchors.fill: parent
                    start: Qt.point(0, 0)
                    end: isHorizontal ? Qt.point(parent.width, 0) : Qt.point(0, parent.width)
                    gradient: root.borderGradient
                }
                visible: false
            }

            Rectangle {
                id: mask
                radius: root.radius
                border.width: root.borderWidth
                anchors.fill: parent
                color: 'transparent'
                visible: false   // otherwise a thin border might be seen.
            }

            OpacityMask {
                id: opM
                anchors.fill: parent
                source: borderFill
                maskSource: mask
            }
        }
    }
}

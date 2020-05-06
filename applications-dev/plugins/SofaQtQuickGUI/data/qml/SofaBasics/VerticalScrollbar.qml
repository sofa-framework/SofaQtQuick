import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0

ScrollBar {
    id: scrollbar
    property var content: parent.contentItem
    policy: parent.height > content.height ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
    height: parent.height - 10
    implicitWidth: 12
    contentItem: GBRect {
        implicitWidth: 12
        implicitHeight: 100
        radius: 6
        border.color: "#3f3f3f"
        LinearGradient {
            cached: true
            source: parent
            anchors.left: parent.left
            anchors.leftMargin: 1
            anchors.right: parent.right
            anchors.rightMargin: 1
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            
            start: Qt.point(0, 0)
            end: Qt.point(12, 0)
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#979797" }
                GradientStop { position: 1.0; color: "#7b7b7b" }
            }
        }
        isHorizontal: true
        borderGradient: Gradient {
            GradientStop { position: 0.0; color: "#444444" }
            GradientStop { position: 1.0; color: "#515151" }
        }
    }
    
    background: GBRect {
        border.color: "#3f3f3f"
        radius: 6
        implicitWidth: 12
        implicitHeight: parent.height
        LinearGradient {
            cached: true
            source: parent
            anchors.left: parent.left
            anchors.leftMargin: 1
            anchors.right: parent.right
            anchors.rightMargin: 1
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            start: Qt.point(0, 0)
            end: Qt.point(12, 0)
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#565656" }
                GradientStop { position: 1.0; color: "#5d5d5d" }
            }
        }
        isHorizontal: true
        borderGradient: Gradient {
            GradientStop { position: 0.0; color: "#444444" }
            GradientStop { position: 1.0; color: "#515151" }
        }
    }
}

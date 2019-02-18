import QtQuick 2.0
import QtQuick.Controls 2.4

ToolButton {
    hoverEnabled: true
    property string iconSource: ""

    Image {
        id: image
        anchors.fill: parent
        source: iconSource
        fillMode: Image.PreserveAspectFit
        Rectangle {
            anchors.fill: parent
            opacity: pressed ? 0.2 : 0.8
            color: pressed ? "white" : "lightgrey"
            visible: pressed || hovered
        }
    }
    background: Rectangle {
        color: "transparent"
    }

}

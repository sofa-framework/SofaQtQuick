import QtQuick 2.2
import QtQuick.Controls 1.0
import SofaBasics 1.0

SofaSceneInterface {
    id: root

    toolbar: Rectangle {
        width: 100
        height: 100
        color: "red"
    }

    toolpanel: Label {
        height: implicitHeight
        text: "Caduceus ToolPanel"

        Rectangle {
            anchors.fill: parent
            z: -1
            color: "lightblue"
        }
    }
}

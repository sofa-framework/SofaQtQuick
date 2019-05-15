import QtQuick 2.12
import QtQuick.Controls 2.4

Rectangle {
    property Gradient highlightcolor: Gradient {
        GradientStop { position: 0.0; color: "#7aa3e5" }
        GradientStop { position: 1.0; color: "#5680c1" }
    }
    property Gradient nocolor: Gradient {
        GradientStop { position: 0.0; color: "transparent" }
        GradientStop { position: 1.0; color: "transparent" }
    }

    property var listView
    property alias text: txtField.text

    id: area
    width: parent.width
    height: 20
    gradient: listView.currentIndex === index ? highlightcolor : nocolor
    Text {
        id: txtField
        leftPadding: 3
        text: modelData
        color: listView.currentIndex === index ? "black" : itemMouseArea.containsMouse ? "lightgrey" : "white"
        anchors.verticalCenter: area.verticalCenter
    }

}

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import QtQuick.Controls.impl 2.4
import QtGraphicalEffects 1.0

Rectangle {
    id: area
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
    property bool hasTemplates: false

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
    Image {
        x: listView.width - width - 10
        y: area.topPadding + (area.availableHeight - height) / 2

        visible: area.hasTemplates
        source: area.hasTemplates ? (itemMouseArea.containsMouse || listView.currentIndex === index ? "qrc:/icon/menuArrowDown.png" : "qrc:/icon/menuArrow.png") : ""
    }
}

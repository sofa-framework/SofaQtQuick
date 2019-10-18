import QtQuick 2.0
import QtQuick.Controls 2.4
import SofaColorScheme 1.0

MenuBar {
    id: menuBar
    height: 30
    implicitHeight: height
    hoverEnabled: true

    delegate: MenuBarItem {
        id: menuBarItem

        function replaceText(txt) {
            var index = txt.indexOf("&");
            if(index >= 0)
                txt = txt.replace(txt.substr(index, 2), ("<u>" + txt.substr(index + 1, 1) +"</u>"));
            return txt;
        }
        contentItem: Text {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 3
            text: replaceText(menuBarItem.text)
            font: menuBarItem.font
            opacity: enabled ? 1.0 : 0.3
            color: "black"
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
        }

        background: Rectangle {
            y: 4
            height: 22
            radius: 4
            opacity: menuBarItem.highlighted ? 0.2 : 0.8
            color: menuBarItem.highlighted ? "lightgrey" : "transparent"
        }
    }

    background: Rectangle {
        implicitWidth: 30
        implicitHeight: 30
        color: hovered ? "#757575" : "#686868"
        border.color: "black"

        GBRect {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 1
            implicitWidth: parent.width
            implicitHeight: parent.implicitHeight - 1
            borderWidth: 1
            borderGradient: Gradient {
                GradientStop { position: 0.0; color: "#7a7a7a" }
                GradientStop { position: 1.0; color: "#5c5c5c" }
            }
            color: "transparent"
        }
    }
}


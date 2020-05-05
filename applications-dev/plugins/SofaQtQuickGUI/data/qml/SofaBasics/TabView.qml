import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TabView {
    id: root
    anchors.fill: parent
    anchors.margins: 4

    style: TabViewStyle {
            frameOverlap: 1
            tab: Rectangle {
                Rectangle {
                    id: innerFrameColor
                    anchors.fill: parent
                    anchors.margins: 1
                    color: "transparent"
                    border.color: "#656565"
                    radius: 2
                }
                Rectangle {
                    x: 2
                    color: parent.color
                    border.color: "transparent"
                    width: parent.width - 4
                    y: 2
                    height: parent.height - 2
                }
                Rectangle {
                    color: innerFrameColor.border.color
                    x: 1
                    width: 1
                    y: 2
                    height: parent.height - 2
                }
                Rectangle {
                    color: innerFrameColor.border.color
                    x: parent.width - 2
                    width: 1
                    y: 2
                    height: parent.height - 2
                }

                Rectangle {
                    color: parent.color
                    x: styleData.index === 0 ? 2 : 0
                    width: parent.width
                    y: parent.height - 1
                    height: 1
                    visible: !styleData.selected
                }
                Rectangle {
                    color: innerFrameColor.border.color
                    x: styleData.index === 0 ? 1 : 0
                    width: parent.width
                    y: parent.height - 2
                    height: 1
                    visible: !styleData.selected
                }
                Rectangle {
                    color: "black"
                    x: 0
                    width: parent.width
                    y: parent.height - 3
                    height: 1
                    visible: !styleData.selected
                }
                Rectangle {
                    color: parent.color
                    width: 2
                    y: parent.height - 1
                    height: 1
                    visible: /*styleData.selected && */styleData.index !== 0
                }
                Rectangle {
                    color: innerFrameColor.border.color
                    x: 0
                    width: 1
                    y: parent.height - 2
                    height: 1
                    visible: /*styleData.selected && */styleData.index !== 0
                }
                Rectangle {
                    color: innerFrameColor.border.color
                    x: parent.width - 1
                    width: 1
                    y: parent.height - 2
                    height: 1
                    visible: styleData.selected
                }
                Rectangle {
                    color: parent.color
                    x: parent.width - 2
                    width: 1
                    y: parent.height - 1
                    height: 1
                    visible: styleData.selected
                }

                color: styleData.selected ? "#49484a" :"#4f4f50"
                border.color: "black"
                implicitWidth: Math.max(text.width + 4, 80)
                implicitHeight: 23
                radius: 2
                Text {
                    id: text
                    anchors.centerIn: parent
                    text: styleData.title
                    color: "#ABABAB"
                }
            }
            frame: Rectangle {

                color: "#49484a"
                anchors.topMargin: -2
                anchors.fill: parent
                border.color: "black"
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1
                    color: "transparent"
                    border.color: "#656565"
                    radius: 2
                }
            }
        }
}

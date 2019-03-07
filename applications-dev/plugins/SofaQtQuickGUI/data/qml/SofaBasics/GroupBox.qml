import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.12
//import SofaApplication 1.0


GroupBox {
    id: control
    title: qsTr("GroupBox")
    implicitWidth: parent.width
    property bool expanded: true
    property int expandedHeight: 0
    onExpandedChanged: {
        contentItem.visible = expanded
        if (!expanded)
        {
            expandedHeight = contentHeight
            contentHeight = 0
        }
        else contentHeight = expandedHeight
    }
    spacing: 6
    leftPadding: 20
    topPadding: 30 + (implicitLabelWidth > 0 ? implicitLabelHeight + spacing : 0)
    bottomPadding: 0

    label: Item {
        Rectangle {
            y: 0
            x: 10
            width: control.width - 20
            height: 1
            color: "#393939"
        }
        Rectangle {
            y: 1
            x: 10
            width: control.width - 20
            height: 1
            color: "#959595"
        }
        Label {
            id: label
            y: 8
            x: control.leftPadding
            width: control.availableWidth
            text: control.title
            color: "black"
            elide: Text.ElideRight
        }
        ColorImage {
            id: groupBoxArrow
            y: 4
            source: control.expanded ? "qrc:/icon/downArrow.png" : "qrc:/icon/rightArrow.png"
            width: 14
            height: 14
            color: "#393939"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    control.expanded = !control.expanded
                }
            }
            anchors.right: label.left
            anchors.verticalCenter: label.verticalCenter
        }
    }
    background: Rectangle {
        visible: control.expanded
        y: control.topPadding - control.bottomPadding
        width: parent.width
        height: parent.height - control.topPadding + control.bottomPadding

        color: "transparent"
    }

}

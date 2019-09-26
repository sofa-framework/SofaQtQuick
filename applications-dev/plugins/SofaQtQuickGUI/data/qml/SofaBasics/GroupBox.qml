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
    property bool expandable: true
    property bool expanded: true
    property int expandedHeight: 0
    property url titleIcon: ""
    property string buttonIconSource: undefined
    property var buttonAction

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
        ColorImage {
            id: groupBoxArrow
            visible: expandable
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
            anchors.verticalCenter: label.verticalCenter
        }
        ColorImage {
            id: titleIconId
            visible: control.titleIcon !== ""
            anchors.verticalCenter: label.verticalCenter
            width: 20
            height: 20
            source: titleIcon
            anchors.left: groupBoxArrow.right
        }
        Label {
            id: label
            y: 8
            x: control.leftPadding
            text: control.title
            color: "black"
            elide: Text.ElideRight
            anchors.left: titleIconId.right
            anchors.leftMargin: 5
        }
        IconButton {
            id: extraButton
            anchors.left: label.right
            anchors.leftMargin: 5
            anchors.verticalCenter: label.verticalCenter
            implicitWidth: 20
            implicitHeight: 20
            visible: true
            iconSource: buttonIconSource
            onClicked: buttonAction()
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

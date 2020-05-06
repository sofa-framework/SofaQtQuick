import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0


ToolTip {
    id: control

    visible: parent.hovered ? parent.hovered : false
    delay: Qt.styleHints.mousePressAndHoldInterval
    font.family: "tahoma"
    timeout: 4000

    onVisibleChanged: {
    }

    property string description: ""
    contentItem: Column {
        anchors.centerIn: parent
        Text {
            id: txt
            text: control.text ? qsTr(control.text) : qsTr(control.description)
            font: control.font
            color: "white"
        }
        Text {
            id: desc
            visible: control.text ? true : false
            Layout.maximumWidth: 250
            text: qsTr(control.description)
            font: control.font
            color: "silver"
            wrapMode: Text.Wrap
        }
    }
}

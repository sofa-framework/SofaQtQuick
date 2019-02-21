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

    implicitHeight: 25

    Component.onCompleted: {
        control.implicitHeight = text && description ? contentHeight + 17 : contentHeight - 5
    }

    onVisibleChanged: {
    }

    property string description: ""
    contentItem: ColumnLayout {
        Text {
            text: control.text ? qsTr(control.text) : qsTr(control.description)
            font: control.font
            color: "white"
        }
        Text {
            visible: control.text ? true : false
            Layout.maximumWidth: 250
            text: qsTr(control.description)
            font: control.font
            color: "silver"
            wrapMode: Text.Wrap
        }
    }
}

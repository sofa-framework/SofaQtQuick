import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

RowLayout {

    id: root

    property alias  r: root.color.r
    property alias  g: root.color.g
    property alias  b: root.color.b
    property alias  a: root.color.a

    property color  color: Qt.rgba(0.5,0.5,0.5,1)
    property int    preferredWidth: 60
    property int    preferredHeight: 20
    property bool   fillWidth: true

    onRChanged: colorPicker.color.r = root.color.r
    onGChanged: colorPicker.color.g = root.color.g
    onBChanged: colorPicker.color.b = root.color.b
    onAChanged: colorPicker.color.a = root.color.a
    onColorChanged: colorPicker.color = root.color

    Rectangle {
        Layout.preferredWidth: root.preferredWidth
        Layout.preferredHeight: root.preferredHeight
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth:  root.fillWidth
        color: root.color
        radius: 2

        MouseArea {
            anchors.fill: parent
            onClicked: colorPicker.open()
        }

        ColorDialog {
            id: colorPicker
            title: "Please choose a color"
            showAlphaChannel: true

            property color previousColor
            Component.onCompleted: {
                previousColor = root.color;
                color = previousColor;
            }

            onColorChanged: root.color = color

            onAccepted: previousColor = color
            onRejected: color = previousColor
        }

        Rectangle {
            anchors.fill: parent
            anchors.margins: 2
            color: Qt.rgba(root.color.r, root.color.g, root.color.b, root.color.a)
            border.color: Qt.rgba(1, 1, 1, 1)
        }
    }

    function setValueFromArray(array) {
        root.color.r = array[0];
        root.color.g = array[1];
        root.color.b = array[2];
        root.color.a = array[3];
    }
}

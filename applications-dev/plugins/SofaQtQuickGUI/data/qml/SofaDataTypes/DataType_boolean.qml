import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.1

Item {
    id: root
    implicitWidth: control.implicitWidth
    implicitHeight: control.implicitHeight

    property var dataObject

    Switch {
        id: control
        anchors.centerIn: parent
        enabled: !dataObject.readOnly
        style: SwitchStyle {
            groove: Rectangle {
                implicitWidth: 60
                implicitHeight: 20
                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    width: parent.width/2
                    height: parent.height
                    color: "#468bb7"
                    Text {
                        color: "white"
                        anchors.centerIn: parent
                        text: "ON"
                    }
                }
                Rectangle {
                    width: parent.width/2
                    height: parent.height
                    anchors.right: parent.right
                    color: "#D3D3D3"
                    Text {
                        color: "white"
                        anchors.centerIn: parent
                        text: "OFF"
                    }
                }
            }
        }

        onCheckedChanged: {
            if(checked !== dataObject.value)
                dataObject.value = checked;
        }

        Binding {
            target: control
            property: "checked"
            value: dataObject.value
            //when: !dataObject.readOnly
        }
    }
}

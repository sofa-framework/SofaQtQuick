import QtQuick 2.5
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import SofaBasics 1.0

Rectangle {
    anchors.fill: parent
    color: "grey"
    GridLayout {
        width: parent.width
        columns: 2
        Label {
            id: lbl1
            text: "Hello World: "
            color: "black"
            Layout.alignment: Qt.AlignVCenter
            leftPadding: 5
        }
        ComboBox {
            id: cbx1
            model: ["Dog", "Cat", "Bunny"]
            Layout.fillWidth: true
        }

        Label {
            id: lbl2
            text: "gravity: "
            color: "black"
            leftPadding: 5
        }
        RowLayout {
            enabled: false
            spacing: -1
            TextField {
                id: txt1
                placeholderText: "x"
                position: cornerPositions["Left"]
                Layout.fillWidth: true
            }
            TextField {
                id: txt2
                placeholderText: "y"
                position: cornerPositions["Middle"]
                Layout.fillWidth: true
            }
            TextField {
                id: txt3
                placeholderText: "z"
                position: cornerPositions["Right"]
                Layout.fillWidth: true
            }
        }



        Label {
            id: lbl3
            text: "BBox: "
            color: "black"
            Layout.alignment: Qt.AlignVCenter
            leftPadding: 5
        }
        ColumnLayout {
            spacing: -1
            RowLayout {
                spacing: -1
                TextField {
                    id: txt21
                    placeholderText: "min x"
                    position: cornerPositions["TopLeft"]
                    Layout.fillWidth: true
                }
                TextField {
                    id: txt22
                    placeholderText: "min y"
                    position: cornerPositions["Middle"]
                    Layout.fillWidth: true
                }
                TextField {
                    id: txt23
                    placeholderText: "min z"
                    position: cornerPositions["TopRight"]
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                spacing: -1
                TextField {
                    id: txt24
                    placeholderText: "max x"
                    position: cornerPositions["BottomLeft"]
                    Layout.fillWidth: true
                }
                TextField {
                    id: txt25
                    placeholderText: "max y"
                    position: cornerPositions["Middle"]
                    Layout.fillWidth: true
                }
                TextField {
                    id: txt26
                    placeholderText: "max z"
                    position: cornerPositions["BottomRight"]
                    Layout.fillWidth: true
                }
            }
        }
    }
}

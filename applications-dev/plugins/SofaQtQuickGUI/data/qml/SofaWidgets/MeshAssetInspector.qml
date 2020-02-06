import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import Asset 1.0
import MeshAsset 1.0

Item {
    id: root
    property Asset selectedAsset

    GroupBox {
        width: root.parent.width
        title: "Mesh properties"
        buttonIconSource: "qrc:/icon/edit.png"
        onButtonClicked: {
            selectedAsset.openThirdParty()
        }
        Column {
            spacing: 2
            Row {
                spacing: 10
                width: root.parent.width
                Label {
                    color: "black"
                    id: verticesLbl
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Vertices: "
                    wrapMode: Text.Wrap
                    width: primitiveTypeLbl.contentWidth
                }
                Label {
                    color: "#eeeeec"
                    id: vertices
                    anchors.verticalCenter: parent.verticalCenter
                    text: selectedAsset.vertices.toString()
                    wrapMode: Text.Wrap
                }
            }
            Row {
                spacing: 10
                width: root.parent.width
                Label {
                    id: facesLbl
                    color: "black"
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Faces: "
                    wrapMode: Text.Wrap
                    width: primitiveTypeLbl.contentWidth
                }
                Label {
                    color: "#eeeeec"
                    id: faces
                    anchors.verticalCenter: parent.verticalCenter
                    text: selectedAsset.faces.toString()
                    wrapMode: Text.Wrap
                }
            }
            Row {
                spacing: 10
                width: root.parent.width
                Label {
                    color: "black"
                    id: materialsLbl
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Materials: "
                    wrapMode: Text.Wrap
                    width: primitiveTypeLbl.contentWidth
                }
                Label {
                    color: "#eeeeec"
                    id: materials
                    anchors.verticalCenter: parent.verticalCenter
                    text: selectedAsset.materials.toString()
                    wrapMode: Text.Wrap
                }
            }
            Row {
                spacing: 10
                width: root.parent.width
                Label {
                    color: "black"
                    id: meshLbl
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Meshes: "
                    wrapMode: Text.Wrap
                    width: primitiveTypeLbl.contentWidth
                }
                Label {
                    color: "#eeeeec"
                    id: mesh
                    anchors.verticalCenter: parent.verticalCenter
                    text: selectedAsset.meshes.toString()
                    wrapMode: Text.Wrap
                }
            }
            Row {
                spacing: 10
                width: root.parent.width
                Label {
                    color: "black"
                    id: primitiveTypeLbl
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Primitive type: "
                    wrapMode: Text.Wrap
                }
                Label {
                    color: "#eeeeec"
                    id: primitiveType
                    anchors.verticalCenter: parent.verticalCenter
                    text: selectedAsset.primitiveType.toString()
                    wrapMode: Text.Wrap
                }
            }
        }
    }
}

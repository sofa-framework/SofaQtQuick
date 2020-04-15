import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import TemplateAsset 1.0

Item {
    id: root
    property TemplateAsset selectedAsset

    GroupBox {
        width: root.parent.width
        title: "Mesh properties"
        buttonIconSource: "qrc:/icon/edit.png"
        onButtonClicked: {
            selectedAsset.openThirdParty()
        }
        Column {
            spacing: 2
            RowLayout {
                spacing: 10
                width: root.parent.width - 40
                Label {
                    color: "black"
                    id: verticesLbl
                    Layout.fillWidth: true
                    text: "Vertices: "
                    wrapMode: Text.Wrap
                    width: primitiveTypeLbl.contentWidth
                }
                Label {
                    color: "#eeeeec"
                    id: vertices
                    text: selectedAsset.callAssetScriptFunction("vertices")
                    wrapMode: Text.Wrap
                }
            }
            RowLayout {
                spacing: 10
                width: root.parent.width - 40
                Label {
                    id: facesLbl
                    color: "black"
                    Layout.fillWidth: true
                    text: "Faces: "
                    wrapMode: Text.Wrap
                    width: primitiveTypeLbl.contentWidth
                }
                Label {
                    color: "#eeeeec"
                    id: faces
                    text: selectedAsset.callAssetScriptFunction("faces")
                    wrapMode: Text.Wrap
                }
            }
            RowLayout {
                spacing: 10
                width: root.parent.width - 40
                Label {
                    color: "black"
                    id: materialsLbl
                    Layout.fillWidth: true
                    text: "Materials: "
                    wrapMode: Text.Wrap
                    width: primitiveTypeLbl.contentWidth
                }
                Label {
                    color: "#eeeeec"
                    id: materials
                    text: selectedAsset.callAssetScriptFunction("materials")
                    wrapMode: Text.Wrap
                }
            }
            RowLayout {
                spacing: 10
                width: root.parent.width - 40
                Label {
                    color: "black"
                    id: meshLbl
                    Layout.fillWidth: true
                    text: "Meshes: "
                    wrapMode: Text.Wrap
                    width: primitiveTypeLbl.contentWidth
                }
                Label {
                    color: "#eeeeec"
                    id: mesh
                    text: selectedAsset.callAssetScriptFunction("meshes")
                    wrapMode: Text.Wrap 
               }
            }
            RowLayout {
                spacing: 10
                width: root.parent.width - 40
                Label {
                    color: "black"
                    id: primitiveTypeLbl
                    Layout.fillWidth: true
                    text: "Primitive type: "
                    wrapMode: Text.Wrap
                }
                Label {
                    color: "#eeeeec"
                    id: primitiveType
                    text: selectedAsset.callAssetScriptFunction("primitiveType")
                    wrapMode: Text.Wrap
                }
            }
        }
    }
}


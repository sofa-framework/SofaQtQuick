import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0
import SofaComponent 1.0
import SofaWidgets 1.0
import Sofa.Core.SofaData 1.0

MenuItem {
    id: renameMenu
    text: "Rename.."
    
    Component {
        id: renameDialog
        Dialog {
            id: dlg
            property var sofaBase
            
            title: "Rename '" + sofaBase.getName() + "'"
            Row {
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Enter the new name: "
                }
                TextField {
                    id: txtField
                    text: sofaBase ? sofaBase.getName() : ""
                    implicitWidth: contentWidth + 20
                }
            }
            onAccepted: {
                sofaBase.setName(txtField.text)
                // collapse / expand the parent node remaps data to the treeView
                treeView.collapse(sceneModel.mapFromSource(currentModelIndex.parent))
                treeView.expand(sceneModel.mapFromSource(currentModelIndex.parent))
            }
            standardButtons: StandardButton.Save | StandardButton.Cancel
            onVisibleChanged: {
                txtField.focus = true
            }
        }
    }
    
    onTriggered: {
        var p = model.getBaseFromIndex(currentModelIndex)
        var d = renameDialog.createObject(renameMenu, {"sofaBase": p})
        d.open()
    }
}

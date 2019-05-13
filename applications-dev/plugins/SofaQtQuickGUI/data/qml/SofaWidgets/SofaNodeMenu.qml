import QtQuick 2.0
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

Menu {
    id: nodeMenu

    function parsePython(c)
    {
        c=c.replace("(","[")
        c=c.replace(")","]")
        c=c.replace(/'/g,'"')
        console.log("PARSE... " + c)
        return JSON.parse(c)
    }

    property var model: null;     ///< The model from which we can get the objects.
    property var currentModelIndex;    ///< The index in the model.
    property bool nodeActivated: true
    property SofaData activated: null
    property string sourceLocation : null
    property string creationLocation : null

    /// Window that contains the object message. The windows is only created when the menu item
    /// is clicked
    SofaWindowComponentMessages { id: windowMessage }

    /// Windows which contains the component creator helper widget with auto-search in
    /// the factory and other database.
    // PopupWindowCreateComponent { id: popupWindowCreateComponent }

    MenuItem {
        text: "Add child"
        onTriggered: {
            var p = model.getBaseFromIndex(currentModelIndex)
            p = p.createChild(p.getNextName("NEWNODE"))
            if(p){
                SofaApplication.signalComponent(p.getPathName());
            }
        }
    }

    MenuItem {
        text: "Add sibling"
        onTriggered: {
            var p = model.getBaseFromIndex(currentModelIndex).getFirstParent()
            p = p.createChild(p.getNextName("NEWNODE"))
            if(p){
                SofaApplication.signalComponent(p.getPathName());
            }
        }
    }

    MenuItem {
        id: addObjectEntry
        text: "Add object..."
        onTriggered: {
            var popupComponent = SofaDataWidgetFactory.getWidget("qrc:/SofaWidgets/PopupWindowCreateComponent.qml")
            var popup2 = popupComponent.createObject(nodeMenu.parent,
                                                     {"sofaNode": model.getBaseFromIndex(currentModelIndex),
                                                      "x" : mouseLoc.mouseX,
                                                      "y" : mouseLoc.mouseY
                                                     });
            popup2.open()
            popup2.forceActiveFocus()
        }
        MouseArea
        {
            id: mouseLoc
            hoverEnabled: true                //< handle mouse hovering.
            anchors.fill : parent
            acceptedButtons: Qt.NoButton      //< forward mouse click.
            propagateComposedEvents: true     //< forward other event.
            z: 0
        }
    }

    MenuItem {
        id: renameMenu
        text: "Rename.."

        Component {
            id: renameDialog
            Dialog {
                id: dlg
                property var sofaBase

                title: "Rename '" + sofaBase.getName() + "'"
                contentItem: TextField {
                    implicitWidth: 200
                    id: txtField
                    text: sofaBase ? sofaBase.getName() : ""
                }
                onAccepted: {
                    sofaBase.setName(txtField.text)
                }
                standardButtons: StandardButton.Save | StandardButton.Cancel
            }
        }


        onTriggered: {
            var p = model.getBaseFromIndex(currentModelIndex)
            var d = renameDialog.createObject(renameMenu, {"sofaBase": p})
            d.open()
        }
    }


    /// Shows a popup with the Data list view.
    MenuItem {
        text: "Data"
        onTriggered: {
            sofaDataListViewWindowComponent.createObject(nodeMenu.parent,
                                                         {"sofaScene": root.sofaScene,
                                                          "sofaComponent": model.getBaseFromIndex(currentModelIndex)});
        }
    }

    MenuItem {
        enabled: model.getBaseFromIndex(currentModelIndex).hasMessage()
        text: "Messages..."
        onTriggered: {
            /// Creates and display an help window object
            windowMessage.createObject(nodeMenu.parent,
                                       {"sofaComponent": model.getBaseFromIndex(currentModelIndex)});
        }
    }

    MenuItem {
        /// todo(dmarchal 2018-15-06) : This should display the content of the description string
        /// provided by Sofa, classname, definition location, declaration location.
        text: "Infos (TODO)"
        onTriggered: {
            console.log("TODO")
        }
    }

    MenuSeparator {}
    MenuItem {
        enabled: creationLocation !== null && creationLocation.length !== 0
        text: "Go to instanciation..."
        onTriggered: {
            var location = parsePython(creationLocation)
            SofaApplication.openInEditor(location[0], location[1])
        }
    }

    MenuItem {
        enabled: sourceLocation !== null && sourceLocation.length !== 0
        text: "Go to implementation..." + sourceLocation.length
        onTriggered: {
            var location = parsePython(sourceLocation)
            SofaApplication.openInEditor(location[0], location[1])
        }
    }

    MenuItem {
        text: "Save as Prefab..."
        onTriggered: {
            var n = model.getBaseFromIndex(currentModelIndex)
            SofaApplication.currentProject.createPrefab(n);
        }
    }

    MenuSeparator {}
    MenuItem {
        enabled: true //(multiparent)?irstparent : true
        text: nodeMenu.nodeActivated ? "Deactivate" : "Activate"
        onTriggered: parent.activated.setValue(nodeMenu.nodeActivated);
    }

    MenuSeparator {}
    MenuItem {
        text: "Delete"
        onTriggered: {
//            var currentRow = model.computeItemRow(currentModelIndex);
            sofaScene.removeComponent(model.getDataFromIndex(currentModelIndex));
//            model.updateCurrentIndex(model.computeModelRow(currentRow));
        }
    }



}

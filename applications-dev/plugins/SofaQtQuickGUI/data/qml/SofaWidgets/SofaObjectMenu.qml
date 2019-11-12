import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaApplication 1.0
import SofaSceneListModel 1.0
import SofaComponent 1.0
import Sofa.Core.SofaData 1.0
import SofaWidgets 1.0
import SofaBasics 1.0
import GraphView 1.0
Menu {

    function parsePython(c)
    {
        c=c.replace("(","[")
        c=c.replace(")","]")
        c=c.replace(/'/g,'"')
        console.log("PARSE... " + c)
        return JSON.parse(c)
    }

    /// Window that contains the object message. The windows is only created when the menu item
    /// is clicked
    SofaWindowComponentMessages { id: windowMessage }

    id: objectMenu

    property var model: null;     ///< The model from which we can get the objects.
    property var currentModelIndex;    ///< The index in the model.
    property var currentObject: model.getBaseFromIndex(currentModelIndex) ;
    property SofaData name: null
    property string sourceLocation : null
    property string creationLocation : null

    MenuItem {
        enabled: true
        text: "Show connections.."
        onTriggered:
        {
            var node = model.getBaseFromIndex(currentModelIndex)
            GraphView.showConnectedComponents(node)
            GraphView.open()
        }
    }


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
        text: "Go to implementation..."
        onTriggered: {
            var location = parsePython(sourceLocation)
            SofaApplication.openInEditor(location[0], location[1])
        }
    }


    MenuItem {
        enabled: currentObject && currentObject.hasMessage()
        text: "Show messages..."
        onTriggered: {
            /// Creates and display an help window object
            windowMessage.createObject(nodeMenu.parent,
                                       {"sofaComponent": model.getBaseFromIndex(currentModelIndex)});
        }
    }




    MenuSeparator {}
    MenuItem {
        text: {
            "Delete object"
        }
        onTriggered: {
            var parent = model.getBaseFromIndex(currentModelIndex.parent);
            var item = model.getBaseFromIndex(currentModelIndex);
            parent.removeObject(item);
        }
    }

    MenuItem {
        text: "Add node..."
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
            var popupComponent = Qt.createComponent("qrc:/SofaWidgets/PopupWindowCreateComponent.qml")
            var popup2 =
                    popupComponent.createObject(nodeMenu.parent,
                                                {
                                                    "sofaNode": model.getBaseFromIndex(currentModelIndex).getFirstParent(),
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

    RenameMenu {
        id: renameMenu
    }
}

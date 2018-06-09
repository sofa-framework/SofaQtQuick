import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0
import SofaWidgets 1.0

Menu {
    id: nodeMenu
    
    property QtObject sofaData: null
    property bool nodeActivated: true



    MenuItem {
        text: "Add child"
        onTriggered: {
            var currentRow = listView.model.computeItemRow(listView.currentIndex);
            sofaScene.addNodeTo(listModel.getComponentById(index));
            listView.updateCurrentIndex(listView.model.computeModelRow(currentRow));
        }
    }

    MenuItem {
        text: "Add component"
        onTriggered: {
        }
    }

    MenuSeparator {}
    
    MenuItem {
        text: {
            nodeMenu.nodeActivated ? "Desactivate" : "Activate"
        }
        onTriggered: listView.model.setDisabled(index, nodeMenu.nodeActivated);
    }

    MenuSeparator {}

    MenuItem {
        text: "Delete"
        onTriggered: {
            var currentRow = listView.model.computeItemRow(listView.currentIndex);
            sofaScene.removeComponent(listModel.getComponentById(index));
            listView.updateCurrentIndex(listView.model.computeModelRow(currentRow));
        }
    }

}

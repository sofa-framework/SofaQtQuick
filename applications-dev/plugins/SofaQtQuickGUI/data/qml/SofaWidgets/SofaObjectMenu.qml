import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaApplication 1.0
import SofaSceneListModel 1.0
import SofaWidgets 1.0

Menu {
    id: objectMenu
    
    property QtObject sofaData: null
    
    MenuItem {
        text: {
            "Delete object"
        }
        onTriggered: {
            var currentRow = listView.model.computeItemRow(listView.currentIndex);
            
            sofaScene.removeComponent(listModel.getComponentById(index));
            
            listView.updateCurrentIndex(listView.model.computeModelRow(currentRow));
        }
    }
}

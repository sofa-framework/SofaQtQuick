import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0

RowLayout {
    id: searchBar
    Layout.fillWidth: true
    Layout.rightMargin: 12
    
    property var filteredRows: []
    
    function updateFilteredRows() {
        searchBar.filteredRows = listModel.computeFilteredRows(searchBarTextField.text);
    }
    
    function previousFilteredRow() {
        updateFilteredRows();
        if(0 === filteredRows.length)
            return;
        
        var i = filteredRows.length - 1;
        for(; i >= 0; --i)
            if(listView.currentIndex > filteredRows[i]) {
                listView.updateCurrentIndex(filteredRows[i]);
                break;
            }
        
        // wrap
        if(-1 == i)
            listView.updateCurrentIndex(filteredRows[filteredRows.length - 1]);
    }
    
    function nextFilteredRow() {
        updateFilteredRows();
        if(0 === filteredRows.length)
            return;
        
        var i = 0;
        for(; i < filteredRows.length; ++i)
            if(listView.currentIndex < filteredRows[i]) {
                listView.updateCurrentIndex(filteredRows[i]);
                break;
            }
        
        // wrap
        if(filteredRows.length === i)
            listView.updateCurrentIndex(filteredRows[0]);
    }

    TextField {
        id: searchBarTextField
        Layout.fillWidth: true
        
        placeholderText: "Search component by name"
        onTextChanged: searchBar.updateFilteredRows()
        
        onAccepted: searchBar.nextFilteredRow();
    }
    Item {
        Layout.preferredWidth: Layout.preferredHeight
        Layout.preferredHeight: searchBarTextField.implicitHeight
        
        IconButton {
            id: previousSearchButton
            anchors.fill: parent
            anchors.margins: 2
            iconSource: "qrc:/icon/previous.png"

            onClicked: searchBar.previousFilteredRow();
        }
    }
    Item {
        Layout.preferredWidth: Layout.preferredHeight
        Layout.preferredHeight: searchBarTextField.implicitHeight

        IconButton {
            id: nextSearchButton
            anchors.fill: parent
            anchors.margins: 2
            iconSource: "qrc:/icon/next.png"
            
            onClicked: searchBar.nextFilteredRow();
        }
    }
    Item {
        Layout.preferredWidth: Layout.preferredHeight
        Layout.preferredHeight: searchBarTextField.implicitHeight
        
        IconButton {
            id: openSourceFile
            anchors.fill: parent
            anchors.margins: 2
            iconSource: "qrc:/icon/leave.png"
            
            onClicked: {
                Qt.openUrlExternally(sofaScene.source);
            }
        }
    }
    
}

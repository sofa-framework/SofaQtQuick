import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.12
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0

RowLayout {
    id: searchBar
    property var sofaScene
    property var selectedItem

    Layout.fillWidth: true
    Layout.rightMargin: 12

    property var model: SofaSceneListModel {
        sofaScene: searchBar.sofaScene
    }
    property var filteredRows: []

    onFilteredRowsChanged: {
        listmodel.clear()
        for (var i = 0 ; i < filteredRows.length ; ++i) {
            var item = searchBar.model.getBaseById(filteredRows[i])
            listmodel.append({"index": filteredRows[i], "name": item.getName(), "path": "@"+item.getPathName(), "component": item})
        }
    }

    TextField {
        id: searchBarTextField
        Layout.fillWidth: true

        placeholderText: "Search component by name"
        Keys.forwardTo: [listView.currentItem, listView]
        onTextEdited: {
            if (popup.opened) {
                searchBar.filteredRows = searchBar.model.computeFilteredRows(text)
            }
            else popup.open()
        }
        onTextChanged: {
            if (popup.opened) {
                searchBar.filteredRows = searchBar.model.computeFilteredRows(text)
            }
            else popup.open()
        }

        onActiveFocusChanged: {
            if (!popup.opened)
                popup.open()
        }

        onAccepted: {
            if (!popup.opened)
                return
            var selectionIndex = listmodel.get(listView.currentIndex).index
            selectedItem = listmodel.get(listView.currentIndex).component
            popup.close()
            var item = searchBar.model.getBaseById(selectionIndex)
            searchBarTextField.text = item.getName()
        }

        onEditingFinished: {
            if (!popup.opened)
                return
            var selectionIndex = listmodel.get(listView.currentIndex).index
            selectedItem = listmodel.get(listView.currentIndex).component
            popup.close()
            var item = searchBar.model.getBaseById(selectionIndex)
            searchBarTextField.text = item.getName()
        }
    }
    Item {
        Layout.preferredWidth: Layout.preferredHeight
        Layout.preferredHeight: searchBarTextField.implicitHeight

        IconButton {
            id: openSourceFile
            anchors.fill: parent
            anchors.margins: 2
            iconSource: "qrc:/icon/ICON_LEAVE.png"
            useHoverOpacity: false
            ColorOverlay {
                source: parent
                anchors.fill: parent
                color: {
                    if (openSourceFile.hovered)
                        return "orange"
                    else if (sofaScene.source !== "undefined")
                        return "#EEEEEE"
                    else
                        return "gray"
                }
            }

            onClicked: {
                SofaApplication.openInEditorFromUrl(sofaScene.source, 0)
            }
        }
    }
    Popup {
        id: popup
        closePolicy: Popup.CloseOnEscape
        visible: searchBarTextField.activeFocus
        padding: 0
        margins: 0
        implicitWidth: searchBarTextField.width
        implicitHeight: contentHeight
        contentWidth: searchBarTextField.width - padding
        contentHeight: listView.implicitHeight < 20 ? 20 : listView.implicitHeight
        y: 20

        ListView {
            id: listView
            visible: searchBarTextField.activeFocus
            anchors.fill: parent
            currentIndex: 0
            implicitHeight: contentHeight
            model: ListModel {
                id: listmodel
            }

            Keys.onDownPressed: {
                listView.incrementCurrentIndex()
            }
            Keys.onUpPressed: {
                listView.decrementCurrentIndex()
            }

            Component {
                id: listViewDelegateID
                Rectangle {
                    id: delegateId
                    property Gradient highlightcolor: Gradient {
                        GradientStop { position: 0.0; color: "#7aa3e5" }
                        GradientStop { position: 1.0; color: "#5680c1" }
                    }
                    property Gradient nocolor: Gradient {
                        GradientStop { position: 0.0; color: "transparent" }
                        GradientStop { position: 1.0; color: "transparent" }
                    }
                    property var view: listView
                    property alias text: entryText.text
                    width: popup.contentWidth
                    height: 20

                    gradient: delegateId.ListView.isCurrentItem ? highlightcolor : nocolor
                    Text {
                        id: entryText
                        color: delegateId.ListView.isCurrentItem ? "black" : itemMouseArea.containsMouse ? "lightgrey" : "white"
                        anchors.verticalCenter: parent.verticalCenter
                        width: popup.contentWidth - x * 2
                        x: 10
                        text: name
                        elide: Qt.ElideLeft
                        clip: true
                    }
                    ToolTip {
                        text: name
                        description: path
                        visible: itemMouseArea.containsMouse
                        timeout: 2000
                    }
                    MouseArea {
                        id: itemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            searchBarTextField.text = entryText.text
                            searchBarTextField.editingFinished()
                        }
                    }
                }
            }

            delegate: listViewDelegateID
        }
    }
}

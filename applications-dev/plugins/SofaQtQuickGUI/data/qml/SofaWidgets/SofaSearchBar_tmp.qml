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
    property var selectionIndex

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
            listmodel.append({"index": filteredRows[i], "name": item.getName(), "path": "@"+item.getPathName()})
        }

    }

    TextField {
        id: searchBarTextField
        Layout.fillWidth: true

        placeholderText: "Search component by name"
        Keys.forwardTo: [listView.currentItem, listView]
        onTextChanged: {
		       searchBar.filteredRows = searchBar.model.computeFilteredRows(text)
		       }		      

        onActiveFocusChanged: {
            if (!popup.opened)
                popup.open()
        }

        //        onAccepted: searchBar.nextFilteredRow();
        onAccepted: {
            print("Accepted: ")
            print(listView.currentIndex)
            print(listmodel.get(listView.currentIndex))
            print(listmodel.get(listView.currentIndex).index)
            var item = searchBar.model.getBaseById(listmodel.get(listView.currentIndex).index)
            searchBarTextField.text = item.getName()
            selectionIndex = listmodel.get(listView.currentIndex).index
            popup.close()
        }

        onEditingFinished: {
            print("Editing finished: ")
            print(listView.currentIndex)
            print(listmodel.get(listView.currentIndex))
            print(listmodel.get(listView.currentIndex).index)
            var item = searchBar.model.getBaseById(listmodel.get(listView.currentIndex).index)
            searchBarTextField.text = item.getName()
            selectionIndex = listmodel.get(listView.currentIndex).index
            popup.close()
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
            keyNavigationEnabled: true
            implicitHeight: contentHeight
            model: ListModel {
                id: listmodel
            }

            Keys.onTabPressed: {
                print("Editing finished: ")
                print(listView.currentIndex)
                print(listmodel.get(listView.currentIndex))
                print(listmodel.get(listView.currentIndex).index)
                var item = searchBar.model.getBaseById(listmodel.get(listView.currentIndex).index)
                searchBarTextField.text = item.getName()
                selectionIndex = listmodel.get(listView.currentIndex).index
                popup.close()
            }
            Keys.onEnterPressed: {
                print("Editing finished: ")
                print(listView.currentIndex)
                print(listmodel.get(listView.currentIndex))
                print(listmodel.get(listView.currentIndex).index)
                var item = searchBar.model.getBaseById(listmodel.get(listView.currentIndex).index)
                searchBarTextField.text = item.getName()
                selectionIndex = listmodel.get(listView.currentIndex).index
                popup.close()
            }

            Keys.onDownPressed: {
                currentIndex++
                if (currentIndex >= listView.rowCount)
                    currentIndex = listView.rowCount - 1
            }

            delegate: Component {
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

                    gradient: view.currentIndex === index ? highlightcolor : nocolor
                    Text {
                        id: entryText
                        color: view.currentIndex === index ? "black" : itemMouseArea.containsMouse ? "lightgrey" : "white"
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
                            view.currentIndex = index
                            searchBarTextField.forceActiveFocus()
                            searchBarTextField.text = entryText.text
                            popup.close()
                        }
                    }
                }
            }
        }
    }
}

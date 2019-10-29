import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import SofaBasics 1.0
import SofaLinkCompletionModel 1.0

ColumnLayout {
    id: control
    property alias text: txtField.text
    property var sofaData
    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 0
    TextField {
        id: txtField
        Layout.fillWidth: true
        Layout.preferredHeight: 20
        placeholderText: sofaData ? "Link: @./path/component." + sofaData.name : ""
        placeholderTextColor: "gray"
        font.family: "Helvetica"
        font.weight: Font.Thin
        text: sofaData ? sofaData.linkPath : ""
        width: parent.width
        clip: true
        borderColor: 0 === sofaData.linkPath.length ? "red" : "#393939"
        Keys.forwardTo: [listView.currentItem, listView]
        onEditingFinished: {
            if (setLinkIfValid(listView.currentItem.text))
                popup.close()

        }
        onTextEdited: {
            if (!popup.opened)
                popup.open()
            if (listView.currentIndex >= completionModel.rowCount())
                listView.currentIndex = 0
            completionModel.linkPath = text
        }
        function setLinkIfValid(value) {
            var oldlink = sofaData.getLinkPath()
            if (sofaData.setLink(value))
                return true
            sofaData.setLink(oldlink)
            return false
        }

        Keys.onReturnPressed: event.accepted = false
        Keys.onEnterPressed: event.accepted = false
        Keys.onPressed:
        {
            if(event.key === Qt.Key_Return)
            {
                txtField.text = listView.currentItem.text
                if (!setLinkIfValid(listView.currentItem.text)) {
                    event.accepted = false
                    event.editingFinished = false
                    event.activeFocus = true
                    focus = false;
                }
                else {
                    popup.close()
                }
            }
        }
    }
    Popup {
        id: popup
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        visible: txtField.activeFocus
        padding: 0
        margins: 0
        implicitWidth: txtField.width
        contentWidth: txtField.width - padding
        contentHeight: listView.implicitHeight < 20 ? 20 : listView.implicitHeight
        y: 20

        ListView {
            id: listView
            visible: txtField.activeFocus
            anchors.fill: parent
            currentIndex: 0
            keyNavigationEnabled: true
            model: SofaLinkCompletionModel {
                id: completionModel
                sofaData: control.sofaData
            }

            delegate: Rectangle {
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
                    text: "@" + completionModel.linkPath + completion
                    elide: Qt.ElideLeft
                    clip: true
                }
                ToolTip {
                    text: name
                    description: help
                    visible: itemMouseArea.containsMouse
                    timeout: 2000
                }
                MouseArea {
                    id: itemMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        view.currentIndex = index
                        txtField.forceActiveFocus()
                        txtField.text = "@"+entryText.text
                    }
                }
            }

        }
    }

}

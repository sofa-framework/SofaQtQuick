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
        onEditingFinished: {
            sofaData.setLink(text)
        }
        onTextEdited: {
            completionModel.linkPath = text
        }

        Keys.onPressed:
        {
            if(event.key === Qt.Key_Return)
            {
                inputField.text = SofaFactory.components[container.currentIndex]
            }
            if(Qt.Key_Down === event.key)
            {
                container.incrementCurrentIndex()
                text = null
            }
            if(Qt.Key_Up === event.key)
            {
                container.decrementCurrentIndex()
                text = null
            }
        }
    }
    Popup {
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        visible: txtField.activeFocus
        contentWidth: txtField.width
        contentHeight: listView.implicitHeight < 20 ? 20 : listView.implicitHeight
        padding: 10
        margins: 0
        y: 20

        ListView {
            id: listView
            visible: txtField.activeFocus
            anchors.fill: parent
            currentIndex: 1
            keyNavigationEnabled: true
            model: SofaLinkCompletionModel {
                id: completionModel
                sofaData: control.sofaData
                onLinkPathChanged: console.log(rowCount())
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

                width: control.width
                height: 20
                gradient: view.currentIndex === index ? highlightcolor : nocolor
                Text {
                    id: entryText
                    color: view.currentIndex === index ? "black" : itemMouseArea.containsMouse ? "lightgrey" : "white"
                    anchors.verticalCenter: parent.verticalCenter
                    x: 3
                    text: completionModel.linkPath + completion
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

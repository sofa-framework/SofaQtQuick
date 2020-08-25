/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import SofaBasics 1.0
import SofaLinkCompletionModel 1.0

/***************************************************************************************************
  *
  * A widget dedicated to edit Data<string> object.
  *
  *************************************************************************************************/
RowLayout {
    id: root
    property var sofaData: null

    property alias text: txtField.text
    TextField {
        id: txtField

        readOnly: false
        implicitWidth: parent.width
        Layout.fillWidth: true
        selectByMouse: true

        placeholderText: root.sofaData ? "Link: @./path/linked_component" : root.sofaData.value.toString()
        placeholderTextColor: "gray"
        font.family: "Helvetica"
        font.weight: Font.Thin
        clip: true
        borderColor: 0 === root.sofaData.value.toString().length ? "red" : "#393939"

        text: root.sofaData.value.toString()
        Keys.forwardTo: [listView.currentItem, listView]
        Connections
        {
            target: root.sofaData
            function onValueChanged(value) {
                text=value.toString()
            }
        }

        onEditingFinished: {
            if (!listView.currentItem) {
                borderColor = "#393939"
                focus = false
                return;
            }

            root.sofaData.value = listView.currentItem.text

            borderColor = "#393939"
            focus = false
        }
        onTextEdited: {
            sofaData.value = text

            if (listView.currentIndex >= completionModel.rowCount())
                listView.currentIndex = 0
            completionModel.linkPath = text
        }
    }
    Popup {
        id: popup
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        visible: txtField.activeFocus
        padding: 0
        margins: 0
        implicitWidth: txtField.width
        implicitHeight: contentHeight
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
                isComponent: true
                sofaData: root.sofaData
                onModelReset: {
                    listView.implicitHeight = completionModel.rowCount() * 20
                    parent.contentHeight = completionModel.rowCount() * 20
                    parent.height = parent.contentHeight
                }
            }
            Keys.onTabPressed: {
                console.log("TAB pressed (in listView)")
                txtField.text = listView.currentItem.text
            }

            Keys.onDownPressed: {
                currentIndex++
                if (currentIndex >= listView.rowCount)
                    currentIndex = listView.rowCount - 1
            }

            delegate: Rectangle {
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

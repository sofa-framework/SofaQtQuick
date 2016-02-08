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

import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaComponent 1.0
import SofaDataListModel 1.0

ListView {
    id: root
    clip: true

    property var sofaScene: SofaApplication.sofaScene
    property var sofaComponent: sofaScene ? sofaScene.selectedComponent : null

    property int nameLabelImplicitWidth : 16

    model: SofaDataListModel {
        id: sofaDataListModel
        sofaComponent: root.sofaComponent

        onSofaComponentChanged: root.nameLabelImplicitWidth = 16;
    }

    section.property: "group"
    section.criteria: ViewSection.FullString
    section.delegate: Rectangle {
        width: parent.width
        height: childrenRect.height
        color: "darkgrey"
        radius: 10

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: section
            color: "darkblue"
            font.bold: true
            font.pixelSize: 14
        }
    }

    delegate: Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height: dataLoader.implicitHeight
        color: index % 2 ? Qt.rgba(0.85, 0.85, 0.85, 1.0) : Qt.rgba(0.9, 0.9, 0.9, 1.0)

        Loader {
            id: dataLoader
            anchors.left: parent.left
            anchors.right: parent.right

            sourceComponent: type === SofaDataListModel.SofaDataType ? sofaDataItem : (type === SofaDataListModel.SofaLinkType ? linkItem : (type === SofaDataListModel.LogType ? logItem : infoItem))

            property Component sofaDataItem: SofaDataItem {
                id: sofaDataItem

                sofaScene: root.sofaScene
                sofaData: sofaDataListModel.getDataById(index)

                nameLabelWidth: root.nameLabelImplicitWidth
                Component.onCompleted: updateNameLabelWidth();
                onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                function updateNameLabelWidth() {
                    root.nameLabelImplicitWidth = Math.max(root.nameLabelImplicitWidth, nameLabelImplicitWidth);
                }
            }

            property Component linkItem: RowLayout {
                spacing: 1

                Text {
                    id: linkNameLabel
                    Layout.preferredWidth: -1 === nameLabelWidth ? implicitWidth : nameLabelWidth
                    Layout.preferredHeight: 20
                    Layout.alignment: Qt.AlignTop

                    text: name
                    font.italic: true
                }
                TextField {
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight
                    readOnly: true

                    text: value.toString().trim()
                    onTextChanged: cursorPosition = 0;
                }

                property int nameLabelWidth: root.nameLabelImplicitWidth
                readonly property int nameLabelImplicitWidth: linkNameLabel.implicitWidth

                Component.onCompleted: updateNameLabelWidth();
                onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                function updateNameLabelWidth() {
                    root.nameLabelImplicitWidth = Math.max(root.nameLabelImplicitWidth, nameLabelImplicitWidth);
                }
            }

            property Component logItem: GridLayout {
                columnSpacing: 1
                rowSpacing: 1
                columns: 2

                Text {
                    id: logNameLabel
                    Layout.preferredWidth: -1 === nameLabelWidth ? implicitWidth : nameLabelWidth
                    Layout.preferredHeight: 20
                    Layout.alignment: Qt.AlignTop

                    text: name
                    font.italic: true
                }
                TextArea {
                    id: logTextArea
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.min(implicitHeight, 400)

                    text: value.toString().trim()
                    onTextChanged: cursorPosition = 0;

                    wrapMode: Text.WordWrap
                    readOnly: true
                }
                Button {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true

                    text: "Clear"
                    onClicked: {
                        if(name === "output") root.sofaComponent.clearOutput();
                        if(name === "warning") root.sofaComponent.clearWarning();
                        logTextArea.text = ""
                    }
                }

                property int nameLabelWidth: root.nameLabelImplicitWidth
                readonly property int nameLabelImplicitWidth: logNameLabel.implicitWidth

                Component.onCompleted: updateNameLabelWidth();
                onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                function updateNameLabelWidth() {
                    root.nameLabelImplicitWidth = Math.max(root.nameLabelImplicitWidth, nameLabelImplicitWidth);
                }
            }

            property Component infoItem: RowLayout {
                spacing: 1

                Text {
                    id: infoNameLabel
                    Layout.preferredWidth: -1 === nameLabelWidth ? implicitWidth : nameLabelWidth
                    Layout.preferredHeight: 20
                    Layout.alignment: Qt.AlignTop

                    text: name
                    font.italic: true
                }
                Text {
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight

                    text: value.toString().trim()
                }

                property int nameLabelWidth: root.nameLabelImplicitWidth
                readonly property int nameLabelImplicitWidth: infoNameLabel.implicitWidth

                Component.onCompleted: updateNameLabelWidth();
                onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                function updateNameLabelWidth() {
                    root.nameLabelImplicitWidth = Math.max(root.nameLabelImplicitWidth, nameLabelImplicitWidth);
                }
            }
        }
    }
}

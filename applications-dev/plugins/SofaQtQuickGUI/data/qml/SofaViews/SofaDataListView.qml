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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaComponent 1.0
import SofaDataListModel 1.0

/*
|--------------------------------------------------------|
| Group 1                                                |
|   Data 1:                                              |
|   Data 2:                                              |
|   Data ...:                                            |
|--------------------------------------------------------|
| Group 2                                                |
|   Data 1:                                              |
|   Data 2:                                              |
|   Data ...:                                            |
|--------------------------------------------------------|
*/
ColumnLayout {
    id: root
    spacing: 0
    enabled: sofaScene ? sofaScene.ready : false

    implicitWidth: 300
    implicitHeight: columnLayout.implicitHeight + reinitButton.implicitHeight

    property var sofaScene: SofaApplication.sofaScene
    property var sofaComponent: sofaScene ? sofaScene.selectedComponent : null

    ScrollView {
        id: scrollView
        Layout.fillWidth: true
        Layout.preferredHeight: Math.min(root.height - reinitButton.implicitHeight, columnLayout.implicitHeight)

        ColumnLayout {
            id: columnLayout
            width: scrollView.width - 9
            spacing: 0

            ListView {
                id: listView
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                clip: true

                property int nameLabelImplicitWidth : 16

                model: SofaDataListModel {
                    id: sofaDataListModel
                    sofaComponent: root.sofaComponent

                    onSofaComponentChanged: listView.nameLabelImplicitWidth = 16;
                }

                section.property: "group"
                section.criteria: ViewSection.FullString
                section.delegate: Rectangle {
                    width: listView.width
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
                    width: listView.width
                    height: dataLoader.implicitHeight
                    color: index % 2 ? Qt.rgba(0.85, 0.85, 0.85, 1.0) : Qt.rgba(0.9, 0.9, 0.9, 1.0)

                    Loader {
                        id: dataLoader
                        width: parent.width

                        sourceComponent: type === SofaDataListModel.SofaDataType ? sofaDataItem : (type === SofaDataListModel.SofaLinkType ? linkItem : (type === SofaDataListModel.LogType ? logItem : infoItem))

                        property Component sofaDataItem: SofaDataItem {
                            id: sofaDataItem

                            sofaScene: root.sofaScene
                            sofaData: sofaDataListModel.getDataById(index)

                            onDoubleClickedOnLabel: sofaDataWindowComponent.createObject(SofaApplication, {"sofaScene": root.sofaScene, "sofaComponent": root.sofaComponent});

                            nameLabelWidth: listView.nameLabelImplicitWidth
                            Component.onCompleted: updateNameLabelWidth();
                            onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                            function updateNameLabelWidth() {
                                listView.nameLabelImplicitWidth = Math.max(listView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                            }
                        }

                        property Component linkItem: RowLayout {
                            spacing: 1

                            Text {
                                id: linkNameLabel
                                Layout.preferredWidth: -1 === nameLabelWidth ? implicitWidth : nameLabelWidth
                                Layout.fillHeight: true
                                Layout.alignment: Qt.AlignTop

                                text: name
                                font.italic: true

                                MouseArea {
                                    anchors.fill: parent
                                    onDoubleClicked: sofaDataWindowComponent.createObject(SofaApplication, {"sofaScene": root.sofaScene, "sofaComponent": root.sofaComponent});
                                }
                            }
                            TextField {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.preferredHeight: implicitHeight
                                readOnly: true

                                text: value.toString().trim()
                                onTextChanged: cursorPosition = 0;
                            }

                            property int nameLabelWidth: listView.nameLabelImplicitWidth
                            readonly property int nameLabelImplicitWidth: linkNameLabel.implicitWidth

                            Component.onCompleted: updateNameLabelWidth();
                            onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                            function updateNameLabelWidth() {
                                listView.nameLabelImplicitWidth = Math.max(listView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                            }
                        }

                        property Component logItem: GridLayout {
                            columnSpacing: 1
                            rowSpacing: 1
                            columns: 2

                            Text {
                                id: logNameLabel
                                Layout.preferredWidth: -1 === nameLabelWidth ? implicitWidth : nameLabelWidth
                                Layout.fillHeight: true
                                Layout.alignment: Qt.AlignTop

                                text: name
                                font.italic: true

                                MouseArea {
                                    anchors.fill: parent
                                    onDoubleClicked: sofaDataWindowComponent.createObject(SofaApplication, {"sofaScene": root.sofaScene, "sofaComponent": root.sofaComponent});
                                }
                            }
                            TextArea {
                                id: logTextArea
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.minimumHeight: Math.min(implicitHeight, 400)

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

                            property int nameLabelWidth: listView.nameLabelImplicitWidth
                            readonly property int nameLabelImplicitWidth: logNameLabel.implicitWidth

                            Component.onCompleted: updateNameLabelWidth();
                            onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                            function updateNameLabelWidth() {
                                listView.nameLabelImplicitWidth = Math.max(listView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                            }
                        }

                        property Component infoItem: RowLayout {
                            spacing: 1

                            Text {
                                id: infoNameLabel
                                Layout.preferredWidth: -1 === nameLabelWidth ? implicitWidth : nameLabelWidth
                                Layout.fillHeight: true
                                Layout.alignment: Qt.AlignTop

                                text: name
                                font.italic: true

                                MouseArea {
                                    anchors.fill: parent
                                    onDoubleClicked: sofaDataWindowComponent.createObject(SofaApplication, {"sofaScene": root.sofaScene, "sofaComponent": root.sofaComponent});
                                }
                            }
                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.preferredHeight: implicitHeight

                                text: value.toString().trim()
                                wrapMode: Text.WordWrap
                            }

                            property int nameLabelWidth: listView.nameLabelImplicitWidth
                            readonly property int nameLabelImplicitWidth: infoNameLabel.implicitWidth

                            Component.onCompleted: updateNameLabelWidth();
                            onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                            function updateNameLabelWidth() {
                                listView.nameLabelImplicitWidth = Math.max(listView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                            }
                        }

                        Component {
                            id: sofaDataWindowComponent

                            Window {
                                id: root
                                width: 400
                                height: 600
                                modality: Qt.NonModal
                                flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
                                visible: true
                                color: "lightgrey"

                                Component.onCompleted: {
                                    width = Math.max(width, loader.implicitWidth);
                                    height = Math.min(height, loader.implicitHeight);
                                }

                                onClosing: destroy();

                                title: sofaComponent ? ("Data of component: " + sofaComponent.getName()) : "No component to visualize"

                                property var sofaScene: root.sofaScene
                                property var sofaComponent: sofaScene ? sofaScene.selectedComponent : null

                                ColumnLayout {
                                    anchors.fill: parent

                                    Loader {
                                        id: loader
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true

                                        onImplicitHeightChanged: root.height = Math.max(root.height, loader.implicitHeight);

                                        sourceComponent: dataLoader.sourceComponent
                                    }

                                    Item {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Label {
                id: emptyLabel
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: !root.sofaComponent

                text: "Select a component to view / edit its data"
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
        }
    }

    Button {
        id: reinitButton
        Layout.fillWidth: true
        visible: root.sofaComponent

        text: "Reinit"
        onClicked: {
            root.sofaComponent.reinit();
            listView.model.update();
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0

ColumnLayout {
    id: root
    spacing: 0
    enabled: sofaScene ? sofaScene.ready : false

    width: 300
    height: searchBar.implicitHeight + listView.contentHeight

    property var sofaScene: SofaApplication.sofaScene
    readonly property var searchBar: searchBar

// search bar

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
    }

    ScrollView {
        Layout.fillWidth: true
        Layout.preferredHeight: Math.min(root.height - searchBar.implicitHeight, listView.contentHeight)
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

        ListView {
            id: listView
            anchors.fill: parent
            clip: true

            currentIndex: -1

            property real rowHeight: 16

            Component.onCompleted: currentIndex = root.sofaScene ? listModel.getComponentId(sofaScene.selectedComponent) : 0

            model: SofaSceneListModel {
                id: listModel
                sofaScene: root.sofaScene
            }

            Connections {
                target: root.sofaScene
                onStatusChanged: {
                    listView.updateCurrentIndex(-1);
                }
                onReseted: {
                    if(listModel)
                        listModel.update();
                }
                onSelectedComponentChanged: listView.updateCurrentIndex();
            }

            function updateCurrentIndex(index) {
                if(updateCurrentIndex.locked)
                    return;

                if(undefined === index)
                    index = listModel.getComponentId(sofaScene.selectedComponent);

                updateCurrentIndex.locked = true;

                listView.currentIndex = -1;

                if(index >= listView.count)
                    index = listView.count - 1;

                if(index >= 0)
                    listView.currentIndex = index;

                updateCurrentIndex.locked = false;
            }

            focus: true

            onCurrentIndexChanged: {
                if(-1 === listView.currentIndex)
                    return;

                var sofaComponent = listModel.getComponentById(listView.currentIndex);
                if(sofaComponent)
                    sofaScene.selectedComponent = sofaComponent;
            }

            onCountChanged: {
                if(currentIndex >= count)
                    currentIndex = 0;
            }

            highlightMoveDuration: 0
            highlight: Rectangle {
                color: "lightsteelblue";
                radius: 5
            }

            delegate: Item {
                anchors.left: parent ? parent.left : undefined
                anchors.right: parent ? parent.right : undefined
                height: visible ? listView.rowHeight : 0
                visible: !(SofaSceneListModel.Hidden & visibility)
                opacity: !(SofaSceneListModel.Disabled & visibility) ? 1.0 : 0.5

                Item {
                    anchors.fill: parent
                    anchors.leftMargin: depth * listView.rowHeight

                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        Image {
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: Layout.preferredWidth
                            visible: undefined !== collapsible ? collapsible : false

                            source: SofaSceneListModel.Collapsed & visibility ? "qrc:/icon/rightArrow.png" : "qrc:/icon/downArrow.png"

                            MouseArea {
                                anchors.fill: parent
                                enabled: undefined !== collapsible ? collapsible : false
                                onClicked: {
                                    var currentRow = listView.model.computeItemRow(listView.currentIndex);
                                    listView.model.setCollapsed(index, !(SofaSceneListModel.Collapsed & visibility))

                                    listView.updateCurrentIndex(listView.model.computeModelRow(currentRow));
                                    root.searchBar.updateFilteredRows();
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.preferredHeight: listView.rowHeight
                            spacing: 2

                            RowLayout {
                                Layout.fillHeight: true
                                spacing: 0

                                Image {
                                    visible: (undefined !== isNode ? isNode : false) && (SofaSceneListModel.Disabled & (undefined !== visibility ? visibility : false))
                                    Layout.preferredWidth: listView.rowHeight
                                    Layout.preferredHeight: Layout.preferredWidth

                                    source: "qrc:/icon/disabled.png"
                                }
                                Rectangle {
                                    id: colorIcon

                                    Layout.preferredWidth: listView.rowHeight * 0.5
                                    Layout.preferredHeight: Layout.preferredWidth

                                    radius: isNode ? Layout.preferredWidth * 0.5 : 0

                                    color: isNode ? "black" : Qt.lighter("#FF" + Qt.md5(type).slice(4, 10), 1.5)
                                    border.width: 1
                                    border.color: "black"
                                }
                                Rectangle {
                                    visible: (undefined !== multiparent ? multiparent : false)
                                    Layout.preferredWidth: listView.rowHeight * 0.5
                                    Layout.preferredHeight: Layout.preferredWidth

                                    radius: isNode ? Layout.preferredWidth * 0.5 : 0

                                    color: colorIcon.color
                                    border.width: 1
                                    border.color: "black"
                                }
                            }

                            Text {
                                text: ((undefined !== isNode ? !isNode : false) && type && 0 !== type.length ? type + " - ": "") + (name && 0 !== name.length ? name : "")
                                color: Qt.darker(Qt.rgba((depth * 6) % 9 / 8.0, depth % 9 / 8.0, (depth * 3) % 9 / 8.0, 1.0), 1.5)
                                font.bold: undefined !== isNode ? isNode : false

                                Layout.fillWidth: true

                                Rectangle {
                                    implicitWidth: parent.implicitWidth
                                    implicitHeight: parent.implicitHeight
                                    z: -1
                                    visible: 0 === searchBar.filteredRows.length ? false : -1 !== searchBar.filteredRows.indexOf(index)

                                    color: "yellow"
                                    opacity: 0.5
                                }

                                Menu {
                                    id: nodeMenu

                                    property QtObject sofaData: null
                                    property bool nodeActivated: true

                                    MenuItem {
                                        text: {
                                            nodeMenu.nodeActivated ? "Desactivate node" : "Activate node"
                                        }
                                        onTriggered: listView.model.setDisabled(index, nodeMenu.nodeActivated);
                                    }

                                    MenuSeparator {}

                                    MenuItem {
                                        text: {
                                            "Delete node"
                                        }
                                        onTriggered: {
                                            var currentRow = listView.model.computeItemRow(listView.currentIndex);

                                            sofaScene.removeComponent(listModel.getComponentById(index));

                                            listView.updateCurrentIndex(listView.model.computeModelRow(currentRow));
                                        }
                                    }
                                }

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

                                MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                                    onClicked: {
                                        if(mouse.button === Qt.LeftButton) {
                                            listView.updateCurrentIndex(index);
                                        } else if(mouse.button === Qt.RightButton) {
                                            var component = listModel.getComponentById(index);

                                            if(isNode) {
                                                nodeMenu.sofaData = component.getComponentData("activated");
                                                nodeMenu.nodeActivated = nodeMenu.sofaData.value();
                                                nodeMenu.popup();
                                            } else {
                                                objectMenu.popup();
                                            }
                                        }
                                    }

                                    onDoubleClicked: {
                                        if(mouse.button === Qt.LeftButton)
                                            sofaDataListViewWindowComponent.createObject(SofaApplication, {"sofaScene": root.sofaScene, "sofaComponent": listModel.getComponentById(index)});
                                    }

                                    Component {
                                        id: sofaDataListViewWindowComponent

                                        Window {
                                            id: root
                                            width: 400
                                            height: 600
                                            modality: Qt.NonModal
                                            flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
                                            visible: true
                                            color: "lightgrey"

                                            Component.onCompleted: {
                                                width = Math.max(width, Math.max(loader.implicitWidth, loader.width));
                                                height = Math.min(height, Math.max(loader.implicitHeight, loader.height));
                                            }

                                            onClosing: destroy();

                                            title: sofaComponent ? ("Data of component: " + sofaComponent.name()) : "No component to visualize"

                                            property var sofaScene: root.sofaScene
                                            property var sofaComponent: sofaScene ? sofaScene.selectedComponent : null

                                            Loader {
                                                id: loader
                                                anchors.fill: parent

                                                sourceComponent: SofaDataListView {

                                                    sofaScene: root.sofaScene
                                                    sofaComponent: root.sofaComponent
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

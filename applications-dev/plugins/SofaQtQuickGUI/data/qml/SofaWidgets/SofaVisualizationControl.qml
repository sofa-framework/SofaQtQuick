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
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaDataTypes 1.0
import SofaDisplayFlagsTreeModel 1.0   

ColumnLayout {
    id: root
    spacing: 0
    enabled: sofaScene ? sofaScene.ready : false

    width: 300
    height: treeView.implicitHeight

    property var sofaScene: SofaApplication.sofaScene

    ScrollView {
        id: scrollView
        Layout.fillWidth: true
        Layout.preferredHeight: Math.min(root.height, treeView.implicitHeight)

        TreeView {
            id: treeView
            implicitWidth: scrollView.width
            implicitHeight: flickableItem.contentHeight

            frameVisible: false
            headerVisible: false

            TableViewColumn {
                title: "Name"
                role: "Name"
                width: root.width - 20
            }

            model: SofaDisplayFlagsTreeModel {
                id: displayFlagsModel

                property bool sceneReady: root.sofaScene && root.sofaScene.ready

                Component.onCompleted: updateVisualStyle();
                onSceneReadyChanged: updateVisualStyle();

                function updateVisualStyle() {
                    if(sceneReady)
                        visualStyleComponent = root.sofaScene.visualStyleComponent();
                }
            }

            Component.onCompleted: expandAll();

            function expandAll() {
                executeVisitor(expand);
            }

            function collapseAll() {
                executeVisitor(collapse);
            }

            function executeVisitor(visitor) {
                var rowCount = model.rowCount();

                var stack = [];
                for(var i = 0; i < rowCount; ++i)
                    stack.push(model.index(i, 0));

                while(0 !== stack.length) {
                    var index = stack.pop();

                    visitor(index);

                    rowCount = model.rowCount(index);
                    for(var i = 0; i < rowCount; ++i)
                        stack.push(model.index(i, 0, index));
                }
            }

            itemDelegate: RowLayout {
                id: layout
                spacing: 2

                CheckBox {
                    id: checkBox
                    Layout.fillHeight: true

                    partiallyCheckedEnabled: false

                    Component.onCompleted: download();
                    onCheckedStateChanged: upload();

                    Connections {
                        target: displayFlagsModel
                        onDataChanged: checkBox.download();
                    }

                    property bool downloading: false

                    function download() {
                        downloading = true;

                        var state = displayFlagsModel.state(styleData.index);

                        if(0 === state) {
                            partiallyCheckedEnabled = false;
                            checkedState = Qt.Unchecked;
                        }
                        else if(1 === state) {
                            partiallyCheckedEnabled = false;
                            checkedState = Qt.Checked;
                        }
                        else {
                            checkedState = Qt.PartiallyChecked;
                        }

                        downloading = false;
                    }

                    function upload() {
                        if(downloading)
                            return;

                        if(Qt.Checked === checkedState)
                            displayFlagsModel.setEnabled(styleData.index);
                        else if(Qt.Unchecked === checkedState)
                            displayFlagsModel.setDisabled(styleData.index);
                    }
                }

                Text {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    anchors.verticalCenter: parent.verticalCenter
                    color: styleData.textColor
                    elide: styleData.elideMode
                    text: styleData.value
                }
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

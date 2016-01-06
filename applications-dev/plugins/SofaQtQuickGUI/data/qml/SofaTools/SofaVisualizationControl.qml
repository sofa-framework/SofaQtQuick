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

CollapsibleGroupBox {
    id: root
    implicitWidth: 0

    title: "Visualization Control"
    property int priority: 90

    property var sofaScene: SofaApplication.sofaScene

    enabled: sofaScene ? sofaScene.ready : false

    implicitHeight: treeView.flickableItem.contentHeight + 47

    TreeView {
        id: treeView
        anchors.fill: parent

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

            var stack = new Array();
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

//    Loader {
//        anchors.fill: parent
//        sourceComponent: displayFlagsComponent
//    }

//    Component {
//        id: displayFlagsComponent

//        TreeView {
//            TableViewColumn {
//                title: "Name"
//                role: "Name"
//                //width: 300
//            }

//            model: SofaDisplayFlagsTreeModel {}

//            Component.onCompleted: {
//                var rowCount = model.rowCount();
//                console.log("rowCount", rowCount);
//                for(var i = 0; i < rowCount; ++i)
//                    expand(model.index(i, 0));

//                resizeColumnsToContents();
//            }

//            itemDelegate: Item {
//                Text {
//                    anchors.verticalCenter: parent.verticalCenter
//                    color: styleData.textColor
//                    elide: styleData.elideMode
//                    text: styleData.value
//                }
//            }
//        }

////        SofaDataType_widget_displayFlags {

////        }
//    }

//    GridLayout {
//        anchors.fill: parent
//        columns: 3

//        Label {
//            Layout.preferredWidth: implicitWidth
//            text: "DT"
//        }
//        SpinBox {
//            id: dtSpinBox
//            Layout.columnSpan: 2
//            Layout.fillWidth: true
//            decimals: 3
//            prefix: value <= 0 ? "Real-time " : ""
//            suffix: " seconds"
//            stepSize: 0.001
//            value: sofaScene ? sofaScene.dt : 0.04
//            onValueChanged: if(sofaScene) sofaScene.dt = value

//            Component.onCompleted: {
//                valueChanged();
//            }
//        }

//        Label {
//            Layout.preferredWidth: implicitWidth
//            text: "Interaction stiffness"
//        }
//        Slider {
//            id: interactionStiffnessSlider
//            Layout.fillWidth: true
//            maximumValue: 1000
//            value: sofaScene ? sofaScene.sofaParticleInteractor.stiffness : 100
//            onValueChanged: if(sofaScene) sofaScene.sofaParticleInteractor.stiffness = value
//            stepSize: 1

//            Component.onCompleted: {
//                minimumValue = 1;
//            }
//        }
//        TextField {
//            Layout.preferredWidth: 40
//            enabled: false
//            text: interactionStiffnessSlider.value
//        }

//        Button {
//            id: displayGraphButton
//            Layout.columnSpan: 3
//            Layout.fillWidth: true
//            text: "Display SofaScene graph"
//            tooltip: ""
//            onClicked: {
//                displayGraphText.text = sofaScene ? sofaScene.dumpGraph() : ""
//                displayGraphDialog.open();
//            }

//            Dialog {
//                id: displayGraphDialog
//                title: "Simulation SofaScene Graph"
//                width: 800
//                height: 600

//                contentItem: Item {
//                    ColumnLayout {
//                        anchors.fill: parent
//                        anchors.margins: 5
//                        TextArea {
//                            id: displayGraphText
//                            Layout.fillWidth: true
//                            Layout.fillHeight: true
//                            readOnly: true

//                            Component.onCompleted: {
//                                wrapMode = TextEdit.NoWrap;
//                            }
//                        }
//                        Button {
//                            Layout.fillWidth: true
//                            text: "Hide"
//                            onClicked: displayGraphDialog.close()
//                        }
//                    }
//                }
//            }
//        }
//    }
}

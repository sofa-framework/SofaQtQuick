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
import QtQuick.Dialogs 1.1
import Qt.labs.settings 1.0
import SofaBasics 1.0
import SofaApplication 1.0
import SofaScene 1.0

ColumnLayout {
    id: root
    spacing: 0

    width: 300
    height: columnLayout.implicitHeight

    property SofaScene sofaScene: SofaApplication.sofaScene
    property alias sofaDataPath: dataPathTextField
    property bool showControl: true

    property int uiId: 0
    property int previousUiId: uiId
    onUiIdChanged: {
        SofaApplication.uiSettings.replace(previousUiId, uiId);
    }

    Settings {
        id: uiSettings
        category: 0 !== root.uiId ? "ui_" + root.uiId : "dummy"
    }

    function init() {

    }

    function load() {
        if(0 === uiId)
            return;


    }

    function setNoSettings() {
        SofaApplication.uiSettings.remove(uiId);
        uiId = 0;
    }

    Component.onCompleted: {
        if(0 === uiId)
            uiId = SofaApplication.uiSettings.generate();
        else
            load();

        init();
    }

    Connections {
        target: sofaScene
        onAboutToUnload:
        {
            dataPathTextField.text = "";
            trackButton.checked = false;
        }
    }

    QtObject {
        id : d

        property QtObject sofaData
    }

    ScrollView {
        id: scrollView
        Layout.fillWidth: true
        Layout.preferredHeight: Math.min(root.height, columnLayout.implicitHeight)

        ColumnLayout {
            id: columnLayout
            width: scrollView.width
            spacing: 0

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0

            // bottom toolbar

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 22
                    visible: root.showControl
                    color: "lightgrey"

                    RowLayout {
                        id: toolBarLayout
                        anchors.fill: parent
                        spacing: 2

                        TextField {
                            id: dataPathTextField
                            Layout.fillWidth: true
                            placeholderText: "@./dataPath from root node"
                            textColor: d.sofaData ? "green" : "black"

                            onTextChanged: d.sofaData = root.sofaScene.data(dataPathTextField.text)
                        }

                        Image {
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: Layout.preferredWidth
                            source: !d.sofaData ? "qrc:/icon/invalid.png" : "qrc:/icon/correct.png"
                        }

                        CheckBox {
                            id: trackButton
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: Layout.preferredWidth
                            Layout.rightMargin: 8
                            checked: false
                            enabled: d.sofaData

                            onClicked: sofaDataItem.updateObject();

                            ToolTip {
                                description: "Track the data value during simulation"
                            }

                            Timer {
                                interval: 50
                                repeat: true
                                running: trackButton.checked

                                onRunningChanged: sofaDataItem.updateObject();
                                onTriggered: sofaDataItem.updateObject();
                            }
                        }
                    }
                }

            // sofa data to display

                SofaDataItem {
                    id: sofaDataItem
                    Layout.fillWidth: true
                    Layout.margins: 2
                    visible: sofaData

                    readOnly: true
                    showName: false
                    showLinkButton: false
                    showTrackButton: false

                    sofaScene: root.sofaScene
                    sofaData: d.sofaData
                }

                Text {
                    id: noDataLabel
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight
                    Layout.alignment: Qt.AlignCenter
                    visible: d.sofaData ? false : true
                    text: "No data at this path"
                    color: "darkred"
                    font.bold: true
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

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
import QtQuick.Dialogs 1.1
import Qt.labs.settings 1.0
import SofaBasics 1.0
import SofaScene 1.0
import "qrc:/SofaCommon/SofaSettingsScript.js" as SofaSettingsScript

Rectangle {
    id: root
    width: 256
    height: 256
    color: "lightgrey"

    property int uiId: 0
    property int previousUiId: uiId
    onUiIdChanged: {
        SofaSettingsScript.Ui.replace(previousUiId, uiId);
    }

    Settings {
        id: uiSettings
        category: 0 !== root.uiId ? "ui_" + root.uiId : "dummy"

        property string sofaSceneSource
        property string sofaDataPath
    }

    function init() {
        uiSettings.sofaSceneSource      = Qt.binding(function() {return sofaSceneSource;});
        uiSettings.sofaDataPath    = Qt.binding(function() {return sofaDataPath;});
    }

    function load() {
        if(0 === uiId)
            return;

        sofaSceneSource     = uiSettings.sofaSceneSource;
        sofaDataPath   = uiSettings.sofaDataPath;
    }

    function setNoSettings() {
        SofaSettingsScript.Ui.remove(uiId);
        uiId = 0;
    }

    Component.onCompleted: {
        if(0 === uiId)
            uiId = SofaSettingsScript.Ui.generate();
        else
            load();

        init();
    }

    property SofaScene sofaScene
    property string sofaSceneSource: ""     // TODO: use this
    property alias sofaDataPath: dataPathTextField
    property bool showControl: true

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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            SofaDataItem {
                id: sofaDataItem
                anchors.fill: parent
                anchors.margins: 2

                readOnly: true
                showName: false
                showLinkButton: false
                showTrackButton: false

                sofaScene: root.sofaScene
                sofaData: d.sofaData
            }

            Text {
                anchors.fill: parent
                anchors.margins: 2
                visible: d.sofaData ? false : true
                text: "No data at this path"
                color: "darkred"
                font.bold: true
                font.pixelSize: 22
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 22
            visible: root.showControl
            color: "lightgrey"

            Flickable {
                id: toolBarFlickable
                anchors.fill: parent
                contentWidth: toolBarLayout.implicitWidth
                leftMargin: 32

                RowLayout {
                    id: toolBarLayout
                    height: parent.height
                    spacing: 2

                    TextField {
                        id: dataPathTextField
                        Layout.fillWidth: true
                        Layout.preferredWidth: toolBarFlickable.width / 2.0
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
                        checked: false
                        enabled: d.sofaData

                        onClicked: sofaDataItem.updateObject();

                        ToolTip {
                            anchors.fill: parent
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
        }
    }
}

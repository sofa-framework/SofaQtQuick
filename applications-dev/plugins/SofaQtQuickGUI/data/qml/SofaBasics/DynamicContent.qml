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

import QtQuick 2.5
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Window 2.2
import Qt.labs.folderlistmodel 2.1
import Qt.labs.settings 1.0
import SofaApplication 1.0

Item {
    id: root
    clip: true

    readonly property bool isDynamicContent: true

    property int uiId: 0
    property int previousUiId: uiId
    onUiIdChanged: {
        SofaApplication.uiSettings.replace(previousUiId, uiId);
    }

    QtObject {
        id: d

        property Timer timer: Timer {
            interval: 200
            running: false
            repeat: false
            onTriggered: errorLabel.visible = true
        }
    }

    Settings {
        id: uiSettings
        category: 0 !== root.uiId ? "ui_" + root.uiId : "dummy"

        property string sourceDir
        property string defaultContentName
        property string currentContentName
        property int    contentUiId
    }

    function init() {
        uiSettings.contentUiId          = Qt.binding(function() {return root.contentUiId;});
        uiSettings.sourceDir            = Qt.binding(function() {return root.sourceDir;});
        uiSettings.defaultContentName   = Qt.binding(function() {return root.defaultContentName;});
        uiSettings.currentContentName   = Qt.binding(function() {return root.currentContentName;});
    }

    function load() {
        if(0 === uiId)
            return;

        root.contentUiId        = uiSettings.contentUiId;
        root.sourceDir          = uiSettings.sourceDir;
        root.defaultContentName = uiSettings.defaultContentName;
        root.currentContentName = uiSettings.currentContentName;
    }

    function setNoSettings() {
        SofaApplication.uiSettings.remove(uiId);
        uiId = 0;
    }

    property string defaultContentName
    property string currentContentName
    property string sourceDir: "qrc:/SofaViews"
    property int    contentUiId: 0

    property var    properties

    onCurrentContentNameChanged: folderListModel.update()
    Component.onCompleted: {
        if(0 === root.uiId) {
            if(parent && undefined !== parent.contentUiId && 0 !== parent.contentUiId) {
                root.uiId = parent.contentUiId;
                load();
            }
            else
                root.uiId = SofaApplication.uiSettings.generate();
        }
        else
            load();

        init();
    }

    FolderListModel {
        id: folderListModel
        nameFilters: ["*.qml"]
        showDirs: false
        showFiles: false
        sortField: FolderListModel.Name
        folder: root.sourceDir

        property var delayedUpdateTimer: Timer {
            interval: 1
            repeat: false
            running: true
            onTriggered: folderListModel.showFiles = true;
        }

        onCountChanged: update();

        property var sceneConnections: Connections {
            target: SofaApplication.sofaScene
            onReadyChanged: if(SofaApplication.sofaScene.ready) folderListModel.refresh();
        }

        function refresh() {
            showFiles = false;
            showFiles = true;
        }

        function update() {
            listModel.clear();

            var contentSet = false;

            var previousIndex = comboBox.currentIndex;
            for(var i = 0; i < count; ++i)
            {
                var fileBaseName = get(i, "fileBaseName");
                var filePath = get(i, "filePath").toString();
                if(-1 !== folder.toString().indexOf("qrc:"))
                    filePath = "qrc" + filePath;

                listModel.insert(i, {"fileBaseName": fileBaseName, "filePath": filePath});

                if(0 === root.currentContentName.localeCompare(fileBaseName)) {
                    comboBox.currentIndex = i;
                    contentSet = true;
                }
            }

            if(!contentSet) {
                for(var i = 0; i < count; ++i)
                {
                    var fileBaseName = get(i, "fileBaseName");

                    if(0 === defaultContentName.localeCompare(fileBaseName)) {
                        comboBox.currentIndex = i;
                        break;
                    }
                }
            }

            if(count > 0 && previousIndex === comboBox.currentIndex)
                loaderLocation.refresh();
        }
    }

    readonly property alias contentItem: loaderLocation.contentItem
    Item {
        anchors.fill: parent

        Rectangle {
            id: toolBar
            color: "lightgrey"

            height: 30
            anchors.left: parent.anchors.left
            anchors.right: parent.anchors.right
            anchors.top: parent.anchors.top

            RowLayout {
                id: toolBarLayout
                spacing: 2

                ComboBox {
                    id: comboBox
                    textRole: "fileBaseName"

                    model: ListModel {
                        id: listModel
                    }

                    onCurrentIndexChanged: {
                        loaderLocation.refresh();
                    }
                }

                /// Open a new windows with this content.
                Button {
                    icon.source: "qrc:/icon/subWindow.png"
                    onClicked: {
                        windowComponent.createObject(SofaApplication, {"source": listModel.get(comboBox.currentIndex).filePath});
                    }

                    Component {
                        id: windowComponent

                        Window {
                            id: window
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

                            property url source

                            Loader {
                                id: loader
                                anchors.fill: parent

                                source: window.source
                            }
                        }
                    }
                }

                Label {
                    id: showAllLabel
                    anchors.right: checkBoxEditting.left
                    anchors.verticalCenter:  toolBar.verticalCenter
                    text: "Edit:"
                }
                CheckBox {
                    id : checkBoxEditting
                    anchors.right:  toolBar.right
                    anchors.verticalCenter:  toolBar.verticalCenter
                }
            }
        }

        Item {
            id: loaderLocation
            anchors.top : toolBar.bottom
            anchors.left : parent.left
            anchors.right : parent.right
            anchors.bottom : parent.bottom
            visible: contentItem

            property Item contentItem

            onContentItemChanged: {
                refreshStandbyItem();
            }

            property string errorMessage
            function refresh() {
                if(-1 === comboBox.currentIndex || comboBox.currentIndex >= listModel.count)
                    return;

                var currentData = listModel.get(comboBox.currentIndex);
                if(currentData) {
                    var source = listModel.get(comboBox.currentIndex).filePath;

                    if(root.currentContentName === comboBox.currentText && null !== loaderLocation.contentItem)
                        return;

                    root.currentContentName = comboBox.currentText;

                    if(loaderLocation.contentItem) {
                        if(undefined !== loaderLocation.contentItem.setNoSettings)
                            loaderLocation.contentItem.setNoSettings();

                        loaderLocation.contentItem.destroy();
                        loaderLocation.contentItem = null;
                    }

                    var contentComponent = Qt.createComponent(source);
                    if(contentComponent.status === Component.Error) {
                        loaderLocation.errorMessage = contentComponent.errorString();
                        refreshStandbyItem();
                    } else {
                        if(0 === root.contentUiId)
                            root.contentUiId = SofaApplication.uiSettings.generate();

                        var contentProperties = root.properties;
                        if(!contentProperties)
                            contentProperties = {};

                        contentProperties["uiId"] = root.contentUiId;
                        contentProperties["anchors.fill"] = loaderLocation;
                        var content = contentComponent.createObject(loaderLocation, contentProperties);

                        if(undefined !== content.uiId)
                            root.contentUiId = Qt.binding(function() {return content.uiId;});
                        else
                        {
                            SofaApplication.uiSettings.remove(root.contentUiId);
                            root.contentUiId = 0;
                        }

                        loaderLocation.contentItem = content;
                    }
                }
            }

            function refreshStandbyItem() {
                if(contentItem) {
                    d.timer.stop();
                    errorLabel.visible = false;
                } else {
                    d.timer.start();
                }
            }
        }


        Rectangle {
            id: errorLabel
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#555555"
            visible: false

            Label {
                anchors.fill: parent
                color: "red"
                visible: 0 !== loaderLocation.errorMessage.length
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: "An error occurred, the content could not be loaded ! Reason: " + loaderLocation.errorMessage
                wrapMode: Text.WordWrap
                font.bold: true
            }
        }
    }
}


//        Image {
//            anchors.bottom: parent.bottom
//            anchors.left: parent.left
//            anchors.bottomMargin: 3
//            anchors.leftMargin: 16
//            source: toolBar.visible ? "qrc:/icon/minus.png" : "qrc:/icon/plus.png"
//            width: 12
//            height: width

//            MouseArea {
//                anchors.fill: parent
//                onClicked: toolBar.visible = !toolBar.visible
//            }
//        }

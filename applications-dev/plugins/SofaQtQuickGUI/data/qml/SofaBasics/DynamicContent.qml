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

//TODO(dmarchal 10/01/2019): move the file model into a separated file
//TODO(dmarchal 10/01/2019): move the drop down menu into a separated file

/// DynamicContent panel, is a widget that contains one of the SofaViews
/// with a menu to select the view to display.
Item {
    id: root
    clip: true

    readonly property bool isDynamicContent: true

    property string defaultContentName
    property string currentContentName
    property string sourceDir: "qrc:/SofaViews"
    property int    contentUiId: 0
    property var    properties

    property int uiId: 0
    property int previousUiId: uiId

    onUiIdChanged: {
        SofaApplication.uiSettings.replace(previousUiId, uiId);
    }

    /// This timer is started if there is a problem while loading the view.
    /// if this happens, the view is reloaded each 200ms in case it is fixed.
    /// TODO(dmarchal 10/01/2019) Isn't there better way to do that ?
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

    ListModel {
        id: listModel
    }

    FolderListModel {
        id: folderListModel
        nameFilters: ["*.qml"]
        showDirs: false
        showFiles: true
        sortField: FolderListModel.Name
        folder: root.sourceDir

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

            console.log("UPDATE LIST FROM FILE");
            var currentContentIndex = comboBox.currentIndex;
            var currentContentName = comboBox.currentText
            listModel.clear();
            var contentSet = false;
            for(var i = 0; i < count; ++i)
            {
                var fileBaseName = get(i, "fileBaseName");
                var filePath = get(i, "filePath").toString();

                if(-1 !== folder.toString().indexOf("qrc:"))
                    filePath = "qrc" + filePath;

                listModel.append({"fileBaseName": fileBaseName, "filePath": filePath});
            }

            /// TODO(dmarchal 10/01/2019) search if filename match the one to display.
            if(!contentSet)
            {
                for(var i = 0; i < listModel.count; ++i)
                {
                    var fileBaseName = get(i, "fileBaseName");
                    if(0 === currentContentName.localeCompare(fileBaseName)) {
                        comboBox.currentIndex = i;
                        break;
                    }
                }
            }
            listModel.reset();
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

                    model: folderListModel
                    currentIndex: 0
                    onCurrentIndexChanged: {
                        loaderLocation.refresh();
                    }

                    //TODO(dmarchal: 10/01/2019 move that into an utilitary file)
                    function findIndex(model, criteria) {
                      for(var i = 0; i < model.count; ++i) { if (criteria(model.get(i))) return i }
                      return null
                    }

                    Component.onCompleted: function()
                    {
                        console.log("COUCOUCOUCOCU")
                        var c = find(model, function(item) {
                            console.log("COMPARE "+ item )

                            return item.fileName === root.currentContentName} );
                        if(c)
                            currentIndex = c

                        console.log("COUCOUCOUCOCU INDEX: " + c)

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

        /// This is the content of the view.
        Item {
            id: loaderLocation
            anchors.top : toolBar.bottom
            anchors.left : parent.left
            anchors.right : parent.right
            anchors.bottom : parent.bottom
            visible: contentItem

            property Item contentItem
            property string errorMessage

            function refresh()
            {
                if(-1 === comboBox.currentIndex || comboBox.currentIndex >= listModel.count)
                    return;

                var currentData = listModel.get(comboBox.currentIndex);
                if(currentData)
                {
                    var source = listModel.get(comboBox.currentIndex).filePath;

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

            function refreshStandbyItem()
            {
                if(contentItem) {
                    d.timer.stop();
                    errorLabel.visible = false;
                } else {
                    d.timer.start();
                }
            }

            onContentItemChanged: {
                refreshStandbyItem();
            }
        }

        /// Error view to indicate that something was wrong while loading the view.
        /// TODO(dmarchal 10/01/2019) Move that into a dedicated component.
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


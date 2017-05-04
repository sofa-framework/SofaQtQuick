/*
Copyright 2017, CNRS

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

Contributors:
    - damien.marchal@univ-lille1.fr

*/

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.2
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
        folderListModel.update();
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
    property string sourceDir: "qrc:/SofaWidgets"
    property int    contentUiId: 0

    property var    properties

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
        id: listModelBase

        ListElement {
            uiName:     "Scene Graph"
            fileBaseName: "SofaSceneGraphView.qml"
            filePath: "qrc:/SofaWidgets/SofaSceneGraphView.qml"
        }
        ListElement {
            uiName: "3D Viewer"
            fileBaseName: "SofaViewer.qml"
            filePath: "qrc:/SofaWidgets/SofaViewer.qml"
        }
        ListElement {
            uiName: "Datas Viewer"
            fileBaseName: "Inspector.qml"
            filePath: "qrc:/SofaWidgets/Inspector.qml"
        }
        ListElement {
            uiName: "Messages"
            fileBaseName: "Message.qml"
            filePath: "qrc:/SofaWidgets/Message.qml"
        }
        ListElement {
            uiName: "Image Viewer"
            fileBaseName: "ImageViewer.qml"
            filePath: "qrc:/SofaWidgets/ImageViewer.qml"
        }
        ListElement {
            uiName: "Scene tools"
            fileBaseName: "SofaSceneToolsPanel.qml"
            filePath: "qrc:/SofaWidgets/SofaSceneToolsPanel.qml"
        }
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

        onCountChanged: {
            update();
        }

        function update() {
            var previousIndex = comboBox.currentIndex

            /// If the count changed...then we can recreated the list.
            if(count==0)
                return;

            /// First fill the listModel with the views we want to have in our application.
            listModel.clear();
            var mapName = {} ;
            for(var i=0;i<listModelBase.count;++i)
            {
                var entry = listModelBase.get(i) ;
                listModel.append(entry) ;
                mapName[entry.filePath] = true ;
            }

            /// Then add the qml files in SofaWidget that are not already in the list
            for(var i = 0; i < count; ++i)
            {
                var fileBaseName = get(i, "fileBaseName");
                var filePath = get(i, "filePath").toString();
                if(-1 !== folder.toString().indexOf("qrc:"))
                    filePath = "qrc" + filePath;

                if(!(filePath in mapName))
                {
                    listModel.append({"uiName" : "_"+fileBaseName+"_",
                                      "fileBaseName": fileBaseName,
                                      "filePath": filePath});
                }
            }

            /// At this point we have a list with all the view we need to expose to the user.
            /// We can now set the index to preserve consistency of the interface.
            for(var i = 0; i < listModel.count; ++i)
            {
                if(0 === currentContentName.localeCompare(listModel.get(i).uiName)) {
                    comboBox.currentIndex = i;
                    break;
                }
            }

            if( previousIndex === comboBox.currentIndex && previousIndex === 0 )
               loaderLocation.refresh();
        }
    }

    readonly property alias contentItem: loaderLocation.contentItem
    Item {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Item {
                id: loaderLocation
                Layout.fillWidth: true
                Layout.fillHeight: true
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

                        root.currentContentName = listModel.get(comboBox.currentIndex).uiName;

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

            Rectangle {
                id: toolBar
                Layout.fillWidth: true
                Layout.preferredHeight: visible ? 22 : 0
                color: "lightgrey"
                visible: false

                Flickable {
                    anchors.fill: parent
                    anchors.leftMargin: 32
                    contentWidth: toolBarLayout.implicitWidth

                    RowLayout {
                        id: toolBarLayout
                        height: parent.height
                        spacing: 2

                        ComboBox {
                            id: comboBox
                            Layout.preferredWidth: 150
                            Layout.preferredHeight: 20
                            textRole: "uiName"
                            style: ComboBoxStyle {}

                            model: listModel ;

                            onCurrentIndexChanged: {
                                loaderLocation.refresh();
                            }

                        }

                        Button {
                            iconSource: "qrc:/icon/subWindow.png"
                            Layout.preferredWidth: Layout.preferredHeight
                            Layout.preferredHeight: 20

                            onClicked: windowComponent.createObject(SofaApplication, {"source": listModel.get(comboBox.currentIndex).filePath});

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
                    }
                }
            }
        }

        Image {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.bottomMargin: 3
            anchors.leftMargin: 16
            source: toolBar.visible ? "qrc:/icon/minus.png" : "qrc:/icon/plus.png"
            width: 12
            height: width

            MouseArea {
                anchors.fill: parent
                onClicked: toolBar.visible = !toolBar.visible
            }
        }
    }
}

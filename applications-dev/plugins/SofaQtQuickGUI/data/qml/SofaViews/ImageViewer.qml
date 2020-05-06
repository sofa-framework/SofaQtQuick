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
import QtQuick.Controls.Styles 1.2
import Qt.labs.folderlistmodel 2.1
import Qt.labs.settings 1.0
import SofaApplication 1.0

ColumnLayout {
    id: root
    spacing: 0

    property int uiId: 0
    property int previousUiId: uiId
    onUiIdChanged: {
        SofaApplication.uiSettings.replace(previousUiId, uiId);
    }

    Settings {
        id: uiSettings
        category: 0 !== root.uiId ? "ui_" + root.uiId : "dummy"

        property string contentDir
        property int index
    }

    function init() {
        uiSettings.contentDir     = Qt.binding(function() {return root.contentDir;});
        uiSettings.index          = Qt.binding(function() {return imageComboBox.currentIndex;});
    }

    function load() {
        if(0 === uiId)
            return;

        root.contentDir             = uiSettings.contentDir;
        imageComboBox.currentIndex  = uiSettings.index;
    }

    function setNoSettings() {
        SofaApplication.uiSettings.remove(uiId);
        uiId = 0;
    }

    Component.onCompleted: {
        if(0 === root.uiId)
            root.uiId = SofaApplication.uiSettings.generate();
        else
            load();

        init();
    }

    property string contentDir: ""

    FileDialog {
        id: openFolderDialog
        title: "Please choose a folder"
        selectFolder: true
        selectMultiple: false
        selectExisting: true
        property var resultTextField    // can be any QtObject containing a "text" property
        onAccepted: {
            root.contentDir = Qt.resolvedUrl(folder);
            imageComboBox.currentIndex = -1;
        }
    }

    FolderListModel {
        id: imageFolderListModel
        folder: contentDir
        showDirs: false
        nameFilters: ["*.png", "*.jpg", "*.jpeg", "*.tga", "*.bmp", "*.ico", "*.pbm", "*.pgm", "*.ppm"]

        onFolderChanged: update()
        onCountChanged: update()

        function update() {
            imageListModel.clear();
            for(var i = 0; i < count; ++i)
                imageListModel.append({"title": get(i, "fileName"), "path": "file:" + get(i, "filePath")});

            if(0 !== count && -1 === imageComboBox.currentIndex)
                imageComboBox.currentIndex = 0;
        }
    }

    ListModel {
        id: imageListModel
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: 64
        color: "black"

        Flickable {
            id: imageContainer
            anchors.fill: parent
            clip: true
            contentWidth: image.width
            contentHeight: image.height
            leftMargin: Math.max(0.0, (width - contentWidth) / 2.0)
            topMargin: Math.max(0.0, (height - contentHeight) / 2.0)

            Image {
                id: image
                source: -1 !== imageComboBox.currentIndex && imageComboBox.currentIndex < imageListModel.count ? imageListModel.get(imageComboBox.currentIndex).path : ""
                fillMode: Image.PreserveAspectFit
            }
        }

        BusyIndicator {
            anchors.fill: parent
            anchors.margins: parent.width / 4
            running: Image.Loading === image.status
        }

        Text {
            anchors.fill: parent
            visible: Image.Null === image.status || Image.Error === image.status
            text: Image.Null === image.status ? "No image to display" : "ERROR: image could not be loaded"
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
        color: "lightgrey"

        Flickable {
            anchors.fill: parent
            anchors.leftMargin: 32
            contentWidth: toolBarLayout.implicitWidth

            RowLayout {
                id: toolBarLayout
                height: parent.height
                spacing: 2

                Item {
                    Layout.preferredWidth: 151
                    Layout.preferredHeight: 20

                    ComboBox {
                        id: imageComboBox
                        anchors.fill: parent
                        anchors.rightMargin: 1
                        textRole: "title"
                        model: imageListModel
                        style: ComboBoxStyle {}
                    }
                }

                Button {
                    Layout.preferredWidth: 25
                    Layout.fillHeight: true
                    iconSource: "qrc:/icon/open.png"

                    onClicked: openFolderDialog.open()
                }

                Button {
                    Layout.preferredWidth: 55
                    Layout.fillHeight: true
                    text: "50%"

                    onClicked: {
                        image.width  = image.sourceSize.width  * 0.5;
                        image.height = image.sourceSize.height * 0.5;
                    }
                }

                Button {
                    Layout.preferredWidth: 55
                    Layout.fillHeight: true
                    text: "100%"

                    onClicked: {
                        image.width  = image.sourceSize.width;
                        image.height = image.sourceSize.height;
                    }
                }

                Button {
                    Layout.preferredWidth: 55
                    Layout.fillHeight: true
                    text: "200%"

                    onClicked: {
                        image.width  = image.sourceSize.width  * 2.0;
                        image.height = image.sourceSize.height * 2.0;
                    }
                }

                Button {
                    Layout.preferredWidth: 55
                    Layout.fillHeight: true
                    text: "Fit"

                    onClicked: {
                        image.width  = Qt.binding(function() {return imageContainer.width;});
                        image.height = Qt.binding(function() {return imageContainer.height;});
                    }
                }
            }
        }
    }
}

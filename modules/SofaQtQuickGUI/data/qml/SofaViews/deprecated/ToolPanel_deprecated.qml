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
import QtQuick.Layouts 1.1
import SofaBasics 1.0
import SofaApplication 1.0
import Qt.labs.folderlistmodel 2.1

ColumnLayout {
    id: root
    spacing: 0
    clip: true

    property var sofaScene: SofaApplication.sofaScene

    FolderListModel {
        id: folderListModel
        folder: "qrc:/SofaViews";
        nameFilters: ["*.qml"]
        showDirs: false
        showFiles: false
        sortField: FolderListModel.Name

        Component.onCompleted: showFiles = true;
        onCountChanged: refresh();
    }

    function refresh() {
        var contentList = [];
        for(var i = 0; i < folderListModel.count; ++i) {
            var baseName = folderListModel.get(i, "fileBaseName");
            var source = "qrc" + folderListModel.get(i, "filePath");

            if("ToolPanel_deprecated" !== baseName && "SofaViewer" !== baseName && "ImageViewer" !== baseName)
                listModel.append({"toolName": baseName, "toolSource": source});
        }

//        for(var i = 0; i < loaderLocation.children.length; ++i)
//            loaderLocation.children[i].destroy();

//        var contentList = [];
//        for(var i = 0; i < folderListModel.count; ++i) {
//            var source = "qrc" + folderListModel.get(i, "filePath");

//            var contentComponent = Qt.createComponent(source);
//            if(contentComponent.status === Component.Error) {
//                console.error("LOADING ERROR:", contentComponent.errorString());
//            } else {
//                contentList.push(contentComponent.createObject(root, {"Layout.fillWidth": true, "sofaScene": sofaScene}));
//            }
//        }

//        contentList.sort(function(a, b) {
//            if(undefined !== a.priority && undefined !== b.priority)
//                return a.priority > b.priority ? -1 : 1;

//            if(undefined === a.priority && undefined !== b.priority)
//                return 1;

//            if(undefined !== a.priority && undefined === b.priority)
//                return -1;

//            return 0;
//        });

//        contextMenu = Qt.createQmlObject("import QtQuick.Controls 1.3; Menu {title: 'Tools'}", root, "contextMenu");
//        for(var i = 0; i < contentList.length; ++i)
//        {
//            contentList[i].parent = loaderLocation;
//            var title = "Tool - " + i;
//            if(contentList[i].title)
//                title = contentList[i].title;

//            var menuItem = contextMenu.addItem(title);
//            menuItem.checkable = true;
//            menuItem.checked = true;

//            var menuSlot = function(content) {return function(checked) {content.visible = checked;}} (contentList[i]);
//            menuItem.toggled.connect(menuSlot);
//        }
    }

    property Menu contextMenu

//    MouseArea {
//        anchors.fill: parent
//        acceptedButtons: Qt.RightButton
//        onClicked: if(contextMenu) contextMenu.popup()
//    }

    ScrollView {
        id: scrollView
        Layout.fillWidth: true
        Layout.preferredHeight: Math.min(root.height, columnLayout.implicitHeight)

        ColumnLayout {
            id: columnLayout
            width: scrollView.width
            height: listView.contentHeight

            ListView {
                id: listView
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                Layout.preferredWidth: contentWidth
                Layout.preferredHeight: contentHeight

                model: ListModel {
                    id: listModel
                }

                delegate: CollapsibleGroupBox {
                    width: listView.width
                    //height: layout.implicitHeight

                    title: toolName

                    ColumnLayout {
                        id: layout
                        anchors.fill: parent
                        //implicitWidth: listView.width
                        //height: implicitHeight

                        Loader {
                            Layout.fillWidth: true
                            Layout.preferredHeight: item ? Math.max(item.implicitHeight, item.height) : 0
                            //Layout.fillHeight: true
                            source: toolSource

//                            sourceComponent : Rectangle {
//                                color: Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0)
//                                implicitHeight: 50
//                                border.width: 5
//                                border.color: "black"
//                            }
                        }

//                        Rectangle {
//                            color: Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0)
//                            Layout.fillWidth: true
//                            Layout.fillHeight: true
//                            height: 50
//                            border.width: 5
//                            border.color: "black"
//                        }
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

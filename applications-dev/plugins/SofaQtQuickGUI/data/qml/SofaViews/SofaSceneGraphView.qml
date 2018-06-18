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
import SofaWidgets 1.0

ColumnLayout {
    id: root
    spacing: 0
    enabled: sofaScene ? sofaScene.ready : false

    width: 300
    height: searchBar.implicitHeight + listView.contentHeight

    property var sofaScene: SofaApplication.sofaScene

    readonly property var searchBar: searchBar
    // search bar
    SofaSearchBar {
        id: searchBar
    }

    Item {
        property bool isActive: false
        property int  index: 0
        id: componentSignaler
        property color color: "blue"
        property QtObject component: null

        Timer {
            id : timer
            interval : 300
            triggeredOnStart : true
            onTriggered: {
                componentSignaler.isActive = running
                if(!running){
                    signallingAnimation.stop()
                    if(componentSignaler.component)
                       sofaScene.selectedComponent = componentSignaler.component
                }
            }
        }

        function start(i,c){
            componentSignaler.component = c
            componentSignaler.index = i
            timer.start()
            signallingAnimation.start()
        }

        ColorAnimation on color {
            id: signallingAnimation
            from: "white"
            to: "lightsteelblue"
            duration: 150
            alwaysRunToEnd: true
            loops: Animation.Infinite
        }
    }

    /// Connect the scenegraph view so that it can be notified when the SofaApplication
    /// is trying to notify that the user is interested to get visual feedback on where componets are.
    Connections {
        target: SofaApplication
        onSignalComponent: {
            var c = sofaScene.component("@"+path)
            if(c){
                var i = listModel.getComponentId(c)
                componentSignaler.start(i, c)
            }
        }
    }

    ScrollView {
        anchors.top: searchBar.bottom
        Layout.fillWidth: true
        Layout.preferredHeight: Math.min(root.height - searchBar.implicitHeight, listView.contentHeight)
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

        ListView {
            id: listView
            anchors.fill: parent
            clip: true

            currentIndex: -1
            highlight: Rectangle {
                color: "lightsteelblue"
                radius: 5
            }

            highlightFollowsCurrentItem: true
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
                onSelectedComponentChanged: {
                    listView.updateCurrentIndex();
                }
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

            delegate: Item {
                anchors.left: parent ? parent.left : undefined
                anchors.right: parent ? parent.right : undefined
                height: visible ? listView.rowHeight : 0
                visible: !(SofaSceneListModel.Hidden & visibility)
                opacity: !(SofaSceneListModel.Disabled & visibility) ? 1.0 : 0.5

                Item {
                    id: item
                    anchors.fill: parent
                    anchors.leftMargin: depth * 12

                    /// The small symbol that can be click to expand/collapse the children of a hierarchy
                    Image {
                        id: itemOpenClose
                        anchors.left : parent.left
                        width: 16
                        height: 16
                        visible:  {
                            if(isNode)
                                return true
                            return false
                        }

                        source: {
                            if(undefined !== multiparent && multiparent)
                            {
                                if(!firstparent)
                                   return "qrc:/icon/linkArrow.png"
                            }
                            if(SofaSceneListModel.Collapsed & visibility)
                                return "qrc:/icon/rightArrow.png"
                            return "qrc:/icon/downArrow.png"
                        }

                        MouseArea {
                            anchors.fill: parent
                            //enabled: isNode ? collapsible : false
                            onClicked: {
                                var currentRow = listView.model.computeItemRow(listView.currentIndex);
                                listView.model.setCollapsed(index, !(SofaSceneListModel.Collapsed & visibility))
                                listView.updateCurrentIndex(listView.model.computeModelRow(currentRow));
                                root.searchBar.updateFilteredRows();
                            }
                        }
                    }

                    RowLayout {
                        anchors.left: itemOpenClose.right
                        spacing: 2

                        Item {
                            implicitHeight: 8
                            implicitWidth: 12
                            Rectangle {
                                id: colorIcon
                                implicitHeight: 8
                                implicitWidth: 8

                                radius: isNode ? implicitHeight*0.5 : 0
                                color: isNode ? "gray" : Qt.lighter("#FF" + Qt.md5(type).slice(4, 10), 1.5)
                                border.width: 1
                                border.color: "black"
                            }
                            Rectangle {
                                anchors.left: colorIcon.left
                                anchors.leftMargin: 4
                                visible: (undefined !== multiparent ? multiparent : false)
                                implicitHeight: 8
                                implicitWidth: 8
                                radius: isNode ? implicitHeight*0.5 : 0
                                color: "gray"
                                opacity: 0.5
                                border.width: 1
                                border.color: "black"
                            }
                            Image {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: colorIcon.verticalCenter
                                height: 16
                                width: 16
                                visible: (undefined !== isNode ? isNode : false) && (SofaSceneListModel.Disabled & (undefined !== visibility ? visibility : false))
                                source: "qrc:/icon/disabled.png"
                                opacity: 1.0
                            }
                        }

                        Text {
                            text: ((undefined !== isNode ? !isNode : false) && type && 0 !== type.length ? type + " - ": "") + (name && 0 !== name.length ? name : "")
                            color: isNode? "black" : Qt.lighter("black",1.2)
                            Layout.fillWidth: true
                            font.italic: (undefined !== multiparent ? !firstparent : false)

                            Rectangle {
                                id: sa
                                implicitWidth: parent.implicitWidth
                                implicitHeight: parent.implicitHeight
                                z: -1
                                visible: {
                                    if( 0 === searchBar.filteredRows.length ){
                                        if( componentSignaler.isActive && index === componentSignaler.index)
                                        {
                                            return true
                                        }
                                        return false;
                                    }
                                    return -1 !== searchBar.filteredRows.indexOf(index)
                                }

                                color: componentSignaler.color
                                opacity: 0.5
                            }

                            SofaNodeMenu
                            {
                                id: nodeMenu
                            }

                            SofaObjectMenu {
                                id: objectMenu
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
                                            if(component.hasLocations()===true)
                                            {
                                                nodeMenu.sourceLocation = component.getSourceLocation()
                                                nodeMenu.creationLocation = component.getCreationLocation()
                                            }
                                            nodeMenu.nodeActivated = nodeMenu.sofaData.value();
                                            nodeMenu.popup();
                                        } else {
                                            objectMenu.popup();
                                        }
                                    }
                                }

                                onDoubleClicked: {
                                    if(mouse.button === Qt.LeftButton){
                                         if(undefined !== multiparent ? multiparent : false){
                                            var srcNode = listModel.getComponentById(index)
                                            var t=sofaScene.component("@"+srcNode.getPathName())
                                            var i=listModel.getComponentId(t)
                                            componentSignaler.start(i, t)
                                         }
                                    }
                                    //sofaDataListViewWindowComponent.createObject(SofaApplication, {"sofaScene": root.sofaScene, "sofaComponent": listModel.getComponentById(index)});
                                }

                                SofaWindowDataListView {
                                    id: sofaDataListViewWindowComponent
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

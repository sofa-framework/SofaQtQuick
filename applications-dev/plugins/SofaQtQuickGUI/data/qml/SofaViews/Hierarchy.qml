/*
Copyright 2018, CNRS

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

import QtQml.Models 2.2
import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneItemModel 1.0
import SofaSceneItemProxy 1.0
import SofaWidgets 1.0

Rectangle {
    id: root
    anchors.fill : parent

    enabled: sofaScene ? sofaScene.ready : false

    property var sofaScene: SofaApplication.sofaScene
    readonly property var searchBar: searchBar

    //    Item {
    //        property bool isActive: false
    //        property int  index: 0
    //        id: componentSignaler
    //        property color color: "blue"
    //        property QtObject component: null

    //        Timer {
    //            id : timer
    //            interval : 300
    //            triggeredOnStart : true
    //            onTriggered: {
    //                componentSignaler.isActive = running
    //                if(!running){
    //                    signallingAnimation.stop()
    //                    if(componentSignaler.component)
    //                        sofaScene.selectedComponent = componentSignaler.component
    //                }
    //            }
    //        }

    //        function start(i,c){
    //            componentSignaler.component = c
    //            componentSignaler.index = i
    //            timer.start()
    //            signallingAnimation.start()
    //        }

    //        ColorAnimation on color {
    //            id: signallingAnimation
    //            from: "white"
    //            to: "lightsteelblue"
    //            duration: 150
    //            alwaysRunToEnd: true
    //            loops: Animation.Infinite
    //        }
    //    }

    //    /// Connect the scenegraph view so that it can be notified when the SofaApplication
    //    /// is trying to notify that the user is interested to get visual feedback on where componets are.
    //    Connections {
    //        target: SofaApplication
    //        onSignalComponent: {
    //            var c = sofaScene.component("@"+path)
    //            if(c){
    //                var i = listModel.getComponentId(c)
    //                componentSignaler.start(i, c)
    //            }
    //        }
    //    }

    // search bar
    SofaSearchBar {
        id: searchBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
    }


    TreeView {
        id : treeView
        anchors.top: searchBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

        selection: ItemSelectionModel {
            model: treeView.model
        }

        onSelectionChanged: {
            console.log("SELECTION CHANGED");
        }

        SofaSceneItemModel
        {
            id: basemodel
            sofaScene: root.sofaScene;
        }

        model:  SofaSceneItemProxy
        {
            id: sceneModel
            model : basemodel
        }

        itemDelegate: Item {
            property bool multiparent : false
            property bool isDisabled : false
            property string name : model ? model.name : ""
            property string typename : model ? model.typename : ""
            property bool isNode: model ? model.isNode : false
            property bool hasMultiParent : model ? model.hasMultiParent : false
            property bool isMultiParent : model ? model.isMultiParent : false
            property bool isSelected: false


            Item {
                id: icon
                anchors.verticalCenter: parent.verticalCenter
                implicitHeight: 8
                implicitWidth: 12


                Rectangle {
                    id: colorIcon
                    implicitHeight: 8
                    implicitWidth: 8

                    radius: isNode ? implicitHeight*0.5 : 0
                    color: isNode ? "gray" : Qt.lighter("#FF" + Qt.md5(typename).slice(4, 10), 1.5)
                    border.width: 1
                    border.color: "black"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            sceneModel.flipComponentVisibility(styleData.index)
                        }
                    }
                }
                Rectangle {
                    anchors.left: colorIcon.left
                    anchors.leftMargin: 4
                    visible: hasMultiParent || isMultiParent
                    implicitHeight: 8
                    implicitWidth: 8
                    radius: isNode ? implicitHeight*0.5 : 0
                    color: "gray"
                    opacity: 0.5
                    border.width: 1
                    border.color: "black"
                }
                //                    Image {
                //                        anchors.horizontalCenter: parent.horizontalCenter
                //                        anchors.verticalCenter: colorIcon.verticalCenter
                //                        height: 16
                //                        width: 16
                //                        visible: (undefined !== isNode ? isNode : false) && (SofaSceneListModel.Disabled & (undefined !== visibility ? visibility : false))
                //                        source: "qrc:/icon/disabled.png"
                //                        opacity: 1.0
                //                    }
            }

            Text {
                anchors.left: icon.right
                anchors.right: parent.right
                color: styleData.textColor
                font.italic: hasMultiParent
                elide: styleData.elideMode
                text: name  //+ "(" + model.row + "/"+ styleData.row + ")"
            }


            SofaNodeMenu
            {
                id: nodeMenu
                model: basemodel
                currentIndex: sceneModel.mapToSource(styleData.index)
            }

            SofaObjectMenu
            {
                id: objectMenu
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    if(mouse.button === Qt.LeftButton)
                    {
                        var srcIndex = sceneModel.mapToSource(styleData.index)
                        sofaScene.selectedComponent = basemodel.getComponentFromIndex(srcIndex)

                        treeView.selection.setCurrentIndex(styleData.index, ItemSelectionModel.ClearAndSelect)
                    } else if(mouse.button === Qt.RightButton) {
                        if(isNode) {
                            //nodeMenu.sofaData = component.getComponentData("activated");
                            //if(component.hasLocations()===true)
                            //{
                            //    nodeMenu.sourceLocation = component.getSourceLocation()
                            //    nodeMenu.creationLocation = component.getCreationLocation()
                            //}
                            //nodeMenu.nodeActivated = nodeMenu.sofaData.value();
                            nodeMenu.popup();
                        } else {
                            objectMenu.popup();
                        }
                    }
                }
            }
        }

        TableViewColumn {
            title: "Hierarchy"
            role: "name"
        }


    }

    CheckBox {
        anchors.top: treeView.anchors.top
        anchors.right: treeView.anchors.right
        anchors.rightMargin: +30
        checked: true
        width : 16
        height : 16
        onCheckedChanged: {
            sceneModel.showOnlyNodes(checked)
        }
    }
}

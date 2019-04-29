/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/

import QtQml.Models 2.2
import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaApplication 1.0
import SofaSceneItemModel 1.0
import SofaSceneItemProxy 1.0
import SofaWidgets 1.0
import SofaBasics 1.0
import SofaColorScheme 1.0
import Qt.labs.settings 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls.Styles 1.4 as QQCS1
import SofaComponent 1.0
import Sofa.Core.Data 1.0

Rectangle {
    id: root
    anchors.fill : parent
    color: SofaApplication.style.contentBackgroundColor
    enabled: sofaScene ? sofaScene.ready : false

    property var sofaScene: SofaApplication.sofaScene


    readonly property var searchBar: searchBar

    /*    //    Item {
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
    //    } */

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
        alternatingRowColors: true
        rowDelegate: Rectangle {
            color: styleData.selected ? "#82878c" : styleData.alternate ? SofaApplication.style.alternateBackgroundColor : SofaApplication.style.contentBackgroundColor
        }
        style: QQCS1.TreeViewStyle {
            headerDelegate: GBRect {
                color: "#757575"
                border.color: "black"
                borderWidth: 1
                borderGradient: Gradient {
                    GradientStop { position: 0.0; color: "#7a7a7a" }
                    GradientStop { position: 1.0; color: "#5c5c5c" }
                }
                height: 20
                width: textItem.implicitWidth
                Text {
                    id: textItem
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: styleData.textAlignment
                    anchors.leftMargin: 12
                    text: styleData.value
                    elide: Text.ElideRight
                    color: textColor
                    renderType: Text.NativeRendering
                }
            }

            branchDelegate: ColorImage {
                id: groupBoxArrow
                y: 1
                source: styleData.isExpanded ? "qrc:/icon/downArrow.png" : "qrc:/icon/rightArrow.png"
                width: 14
                height: 14
                color: "#393939"
            }
            backgroundColor: SofaApplication.style.contentBackgroundColor

            scrollBarBackground: GBRect {
                border.color: "#3f3f3f"
                radius: 6
                implicitWidth: 12
                implicitHeight: 12
                LinearGradient {
                    cached: true
                    source: parent
                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.right: parent.right
                    anchors.rightMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    start: Qt.point(0, 0)
                    end: Qt.point(12, 0)
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#565656" }
                        GradientStop { position: 1.0; color: "#5d5d5d" }
                    }
                }
                isHorizontal: true
                borderGradient: Gradient {
                    GradientStop { position: 0.0; color: "#444444" }
                    GradientStop { position: 1.0; color: "#515151" }
                }
            }

            corner: null
            scrollToClickedPosition: true
            transientScrollBars: false
            frame: Rectangle {
                color: "transparent"
            }

            handle: Item {
                implicitWidth: 12
                implicitHeight: 12
                GBRect {
                    radius: 6
                    anchors.fill: parent
                    border.color: "#3f3f3f"
                    LinearGradient {
                        cached: true
                        source: parent
                        anchors.left: parent.left
                        anchors.leftMargin: 1
                        anchors.right: parent.right
                        anchors.rightMargin: 1
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0

                        start: Qt.point(0, 0)
                        end: Qt.point(12, 0)
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "#979797" }
                            GradientStop { position: 1.0; color: "#7b7b7b" }
                        }
                    }
                    isHorizontal: true
                    borderGradient: Gradient {
                        GradientStop { position: 0.0; color: "#444444" }
                        GradientStop { position: 1.0; color: "#515151" }
                    }

                }
            }
            incrementControl: Rectangle {
                visible: false
            }
            decrementControl: Rectangle {
                visible: false
            }
        }
        selection: ItemSelectionModel {
            model: treeView.model
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

            onModelHasReset: {
                treeView.restoreNodeState()
            }
        }

        Settings {
            id: nodeSettings
            category: "Hierarchy"
            property var nodeState: ({})
        }

        function getExpandedState()
        {
            var nsArray = SofaApplication.nodeSettings.nodeState.split(';')
            for (var idx in nsArray)
            {
                if (nsArray[idx] !== "")
                {
                    var key = nsArray[idx].split(":")[0]
                    var value = nsArray[idx].split(":")[1]
                    nodeSettings.nodeState[key] = value
                }
            }
        }

        function restoreNodeState() {
            if (Object.keys(nodeSettings.nodeState).length === 0 && SofaApplication.nodeSettings.nodeState !== "")
                getExpandedState()
            for (var key in nodeSettings.nodeState) {
                if (nodeSettings.nodeState[key] === "1")
                {
                    var idx = null
                    idx = sceneModel.mapFromSource(basemodel.getIndexFromBase(sofaScene.node(key)))
                    treeView.expand(idx)
                }
            }
        }

        function storeExpandedState(index) {
            var srcIndex = sceneModel.mapToSource(index)
            var theComponent = basemodel.getBaseFromIndex(srcIndex)
            nodeSettings.nodeState[theComponent.getPathName() !== "" ? theComponent.getPathName() : "/"] = treeView.isExpanded(index)
            var i = 0;
            SofaApplication.nodeSettings.nodeState = ""
            for (var key in nodeSettings.nodeState) {
                if (i !== 0)
                    SofaApplication.nodeSettings.nodeState += ";"
                SofaApplication.nodeSettings.nodeState += key + ":" + (nodeSettings.nodeState[key] ? "1" : "0")
                i++
            }
        }

        onExpanded: {
            storeExpandedState(index)
        }
        onCollapsed: {
            storeExpandedState(index)
        }

        itemDelegate: Item {
            id: itemDelegateID
            property bool multiparent : false
            property bool isDisabled : false
            property bool isSelected: false
            property string name : model && model.name ? model.name : ""
            property string typename : model && model.typename ? model.typename : ""
            property bool isNode: model && model.isNode ? model.isNode : false
            property bool hasMultiParent : model && model.hasMultiParent ? model.hasMultiParent : false
            property bool isMultiParent : model && model.isMultiParent ? model.isMultiParent : false

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
                //                //                    Image {
                //                //                        anchors.horizontalCenter: parent.horizontalCenter
                //                //                        anchors.verticalCenter: colorIcon.verticalCenter
                //                //                        height: 16
                //                //                        width: 16
                //                //                        visible: (undefined !== isNode ? isNode : false) && (SofaSceneListModel.Disabled & (undefined !== visibility ? visibility : false))
                //                //                        source: "qrc:/icon/disabled.png"
                //                //                        opacity: 1.0
                //                //                    }
            }

            Text {
                anchors.left: icon.right
                anchors.right: parent.right
                color: styleData.textColor
                font.italic: hasMultiParent
                elide: styleData.elideMode
                text: name //+ "(" + model.row + "/"+ styleData.row + ")"
            }

//            MouseArea
//            {
//                id: mouseAreaItemID
//                anchors.fill: parent
//                acceptedButtons: Qt.LeftButton | Qt.RightButton
//                onClicked:
//                {
//                    var srcIndex = sceneModel.mapToSource(styleData.index)
//                    var theComponent = basemodel.getDataFromIndex(srcIndex)

//                    if(mouse.button === Qt.LeftButton)
//                    {
//                        sofaScene.selectedComponent = theComponent
//                        treeView.selection.setCurrentIndex(styleData.index, ItemSelectionModel.ClearAndSelect)
//                    } else if(mouse.button === Qt.RightButton) {
//                        if(isNode)
//                        {
//                            nodeMenu.sofaData = theComponent.getComponentData("activated");
//                            if(theComponent.hasLocations()===true)
//                            {
//                                nodeMenu.sourceLocation = theComponent.getSourceLocation()
//                                nodeMenu.creationLocation = theComponent.getCreationLocation()
//                            }
//                            nodeMenu.nodeActivated = nodeMenu.sofaData.value;
//                            nodeMenu.popup();
//                        } else {
//                            objectMenu.popup();
//                        }
//                    }
//                }
//            }
        }
        QQC1.TableViewColumn {
            title: "Hierarchy"
            role: "name"

        }

        SofaNodeMenu
        {
            id: nodeMenu
            model: basemodel
            currentModelIndex: undefined
        }

        SofaObjectMenu
        {
            id: objectMenu
            model: basemodel
            currentModelIndex: undefined
        }

        mouser.acceptedButtons: Qt.LeftButton | Qt.RightButton

        mouser.onClicked:
        {
            var srcIndex = sceneModel.mapToSource(treeView.selection.currentIndex)
            var theComponent = basemodel.getBaseFromIndex(srcIndex)
            if(mouse.button === Qt.LeftButton)
            {
                sofaScene.selectedComponent = theComponent
            } else if (mouse.button === Qt.RightButton) {
                if(theComponent.isNode())
                {
                    nodeMenu.currentModelIndex = srcIndex
                    nodeMenu.activated = theComponent.getData("activated");
                    if(theComponent.hasLocations()===true)
                    {
                        nodeMenu.sourceLocation = theComponent.getSourceLocation()
                        nodeMenu.creationLocation = theComponent.getInstanciationLocation()
                    }
                    nodeMenu.nodeActivated = nodeMenu.activated.value;
                    nodeMenu.popup();
                } else {
                    if(theComponent.hasLocations()===true)
                    {
                        objectMenu.sourceLocation = theComponent.getSourceLocation()
                        objectMenu.creationLocation = theComponent.getInstanciationLocation()
                    }
                    objectMenu.currentModelIndex = srcIndex
                    objectMenu.name = theComponent.getData("name");
                    objectMenu.popup();
                }
            }
        }

//        mouser.onPressAndHold: {
//            overlay.visible = true
//            overlay.implicitHeight = treeView.indexAt(0, y).height
//            overlay.implicitWidth = treeView.indexAt(0, y).width
//            overlay.anchors.verticalCenter = treeView.indexAt(0, y).verticalCenter
//            overlay.anchors.horizontalCenter = treeView.indexAt(0, y).horizontalCenter
//            console.error(overlay.width + " " + overlay.height)
//        }
//        mouser.onReleased: {
//            overlay.visible = false
//            parent = overlay.Drag.target !== null ? overlay.Drag.target : mouser
//        }
//        mouser.drag.target: overlay
//        Rectangle {
//            id: overlay
//            color: "#50FFF000"

//            visible: false
//            Drag.active: mouseAreaItemID.drag.active
//            Drag.hotSpot.x: implicitWidth / 2
//            Drag.hotSpot.y: implicitHeight / 2
//            states: State {
//                when: mouseAreaItemID.drag.active
//                ParentChange { target: overlay; parent: treeView }
//                AnchorChanges { target: overlay; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
//            }
//        }

    }

    Label {
        anchors.right: nodesCheckBox.left
        anchors.bottom: nodesCheckBox.bottom
        text: "Show only nodes: "
    }

    CheckBox {
        id: nodesCheckBox
        anchors.top: treeView.anchors.top
        anchors.topMargin: 1
        anchors.right: treeView.anchors.right
        anchors.rightMargin: +30
        checked: false
        onCheckedChanged: {
            sceneModel.showOnlyNodes(checked)
        }
    }
}

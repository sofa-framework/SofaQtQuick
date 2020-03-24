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
import Sofa.Core.SofaData 1.0
import Sofa.Core.SofaNode 1.0
import Sofa.Core.SofaBase 1.0

Rectangle {
    id: root
    anchors.fill : parent
    color: SofaApplication.style.contentBackgroundColor
    enabled: SofaApplication.sofaScene ? SofaApplication.sofaScene.ready : false

    readonly property var searchBar: searchBar

    Item {

        //    /// Connect the scenegraph view so that it can be notified when the SofaApplication
        //    /// is trying to notify that the user is interested to get visual feedback on where componets are.
        Connections {
            target: SofaApplication
            onSignalComponent: function(objectpath){
                var c = SofaApplication.sofaScene.get(objectpath)
                if(c)
                {
                    var baseIndex = basemodel.getIndexFromBase(c)
                    var sceneIndex = sceneModel.mapFromSource(baseIndex)
                    treeView.expandAncestors(sceneIndex)
                    treeView.selection.setCurrentIndex(sceneIndex, ItemSelectionModel.ClearAndSelect);
                }
            }
        }
    }

    // search bar
    SofaSearchBar {
        id: searchBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        sofaScene: SofaApplication.sofaScene
        onSelectedItemChanged: {
            var baseIndex = basemodel.getIndexFromBase(selectedItem)
            var sceneIndex = sceneModel.mapFromSource(baseIndex)
            treeView.expandAncestors(sceneIndex)
            treeView.selection.setCurrentIndex(sceneIndex, ItemSelectionModel.ClearAndSelect);
        }
    }

    TreeView {
        id : treeView
        anchors.top: searchBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        verticalScrollBarPolicy: Qt.ScrollBarAsNeeded
        alternatingRowColors: true


        rowDelegate: Rectangle {
            color: styleData.selected ? "#82878c" : styleData.alternate ? SofaApplication.style.alternateBackgroundColor : SofaApplication.style.contentBackgroundColor
        }

        headerDelegate: Rectangle {
            x: 5
            y: 2
            height: 18
            color: SofaApplication.style.contentBackgroundColor
            property var pressed: styleData.pressed
            onPressedChanged: forceActiveFocus()
            Label {
                color: "black"
                text: styleData.value
            }

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
            id: selectionModel
            model: treeView.model
            onSelectionChanged:
            {
                var srcIndex = sceneModel.mapToSource(currentIndex)
                var theComponent = basemodel.getBaseFromIndex(srcIndex)
                SofaApplication.selectedComponent = theComponent
                SofaApplication.currentProject.selectedAsset = null
            }
        }

        // Expands all ancestors of index
        function expandAncestors(index) {
            var idx = index.parent
            var old_idx = index
            while(idx && old_idx !== idx)
            {
                var srcIndex = sceneModel.mapToSource(idx)
                var theComponent = basemodel.getBaseFromIndex(srcIndex)
                if (theComponent === null) break;
                //console.error("Just expanded: " + theComponent.getPathName())
                // On dépile récursivement les parents jusqu'à root
                treeView.expand(idx)
                old_idx = idx
                idx = idx.parent
            }
        }
        // Collapses all ancestors of index
        function collapseAncestors(index) {
            var idx = index.parent
            var old_idx = index
            while(idx && old_idx !== idx)
            {
                var srcIndex = sceneModel.mapToSource(idx)
                var theComponent = basemodel.getBaseFromIndex(srcIndex)
                if (theComponent === null) break;
                // On dépile récursivement les parents jusqu'à root
                treeView.collapse(idx)
                old_idx = idx
                idx = idx.parent
            }
        }

        SofaSceneItemModel
        {
            id: basemodel
            sofaScene: SofaApplication.sofaScene;
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
            treeView.selection.select(treeView.selection.currentIndex, selectionModel.Deselect)
            if (Object.keys(nodeSettings.nodeState).length === 0 && SofaApplication.nodeSettings.nodeState !== "") {
                getExpandedState()
            }
            for (var key in nodeSettings.nodeState) {
                if (nodeSettings.nodeState[key] === "1")
                {
                    var idx = null
                    idx = sceneModel.mapFromSource(basemodel.getIndexFromBase(SofaApplication.sofaScene.node(key)))
                    treeView.expand(idx)
                    expandAncestors(idx);
                }
            }
        }

        function storeExpandedState(index)
        {
            var srcIndex = sceneModel.mapToSource(index)
            var theComponent = basemodel.getBaseFromIndex(srcIndex)
            if (theComponent === null) return;
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
            storeExpandedState(currentIndex)
        }
        onCollapsed: {
            var srcIndex = sceneModel.mapToSource(currentIndex)
            var theComponent = basemodel.getBaseFromIndex(srcIndex)
            storeExpandedState(currentIndex)
        }

        itemDelegate: Rectangle {
            id: itemDelegateID


            property string origin: "Hierarchy"
            property bool multiparent : false
            property bool isDisabled : false
            property var renaming: false
            property string name : model && model.name ? model.name : ""
            property string typename : model && model.typename ? model.typename : ""
            property string shortname : model && model.shortname ? model.shortname : ""
            property bool isNode: model && model.isNode ? model.isNode : false
            property bool hasMultiParent : model && model.hasMultiParent ? model.hasMultiParent : false
            property bool isMultiParent : model && model.isMultiParent ? model.isMultiParent : false
            property bool hasMessage : model && testForMessage(styleData.index, styleData.isExpanded)
            property bool hasChildMessage : model && testForChildMessage(styleData.index, styleData.isExpanded)
            property string statusString: model && model.statusString
            property var index: styleData.index
            property var tmpParent

            Connections {
                target: treeView
                onCurrentIndexChanged: {
                    var srcIndex = sceneModel.mapToSource(treeView.currentIndex)
                    var treeViewComponent = basemodel.getBaseFromIndex(srcIndex)
                    srcIndex = sceneModel.mapToSource(styleData.index)
                    var component = basemodel.getBaseFromIndex(srcIndex)
                    if (!component || !treeViewComponent) return;
                    if (treeViewComponent.getPathName() === component.getPathName())
                        mouseArea.forceActiveFocus()
                }
            }

            color: "transparent"
            function getIconFromStatus(s)
            {
                if(s === "Undefined")
                    return "qrc:/icon/state_bubble_1.png"
                if(s === "Loading")
                    return "qrc:/icon/state_bubble_3.png"
                if(s === "Busy")
                    return "qrc:/icon/state_bubble_3.png"
                if(s === "Valid")
                    return "qrc:/icon/state_bubble_4.png"
                if(s === "Ready")
                    return "qrc:/icon/state_bubble_4.png"
                if(s === "Invalid")
                    return "qrc:/icon/state_bubble_5.png"

                return "qrc:/icon/state_bubble_1.png"
            }

            function testForChildMessage(index, isExpanded)
            {
                var srcIndex = sceneModel.mapToSource(index)
                var c = basemodel.getBaseFromIndex(srcIndex)

                if ( c===null )
                    return false

                if( !model.isNode )
                    return false

                return c.hasMessageInChild(c)
            }

            function testForMessage(index, isExpanded)
            {
                var srcIndex = sceneModel.mapToSource(index)
                var c = basemodel.getBaseFromIndex(srcIndex)

                if ( c===null )
                    return false
                return c.hasMessage();
            }

            function getFirstChildWithMessage(index)
            {
                var srcIndex = sceneModel.mapToSource(index)
                var c = basemodel.getBaseFromIndex(srcIndex)

                if ( c===null )
                    return null
                return c.getFirstChildWithMessage()
            }

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

                    Image {
                        id: prefabIcon
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        height: 12
                        width: 12
                        visible:
                        {
                            var srcIndex = sceneModel.mapToSource(styleData.index)
                            var c = basemodel.getBaseFromIndex(srcIndex)
                            c!==null && isNode && c.isPrefab()
                        }
                        source: "qrc:/icon/ICON_PREFAB.png"
                        opacity: 1.0
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
            }

            Component {
                id: textComponent
                Text {
                    id: rowText
                    color: styleData.textColor
                    font.italic: hasMultiParent
                    elide: Text.ElideRight
                    clip: true
                    text: {
                        if (isNode || typename == name)
                            return name
                        else if (name == shortname)
                            return name
                        else
                            return typename+" ("+name+")"
                    }
                }
            }

            Component {
                id: renamingTextComponent
                TextField {
                    id: renamingText
                    text: name
                    enabled: true
                    selectByMouse: true
                    function forceFocus() {
                        selectAll()
                        forceActiveFocus()
                    }
                    Component.onCompleted: {
                        forceFocus()
                    }
                    onEditingFinished: {
                        var srcIndex = sceneModel.mapToSource(index)
                        var c = basemodel.getBaseFromIndex(srcIndex)
                        if (c.rename(text))
                            renaming = false
                        else
                            forceFocus()
                    }
                }
            }
            Loader {
                id: textLoader
              // Explicitly set the size of the
              // Loader to the parent item's size
                anchors.left: icon.right
                anchors.right: parent.right
                anchors.rightMargin: 40
                sourceComponent: {
                    return renaming ? renamingTextComponent : textComponent
                }
            }


            Image {
                id: componentState
                anchors.verticalCenter: textLoader.verticalCenter
                anchors.right: parent.right
                height: 12
                width: 12
                visible: true
                source: getIconFromStatus(statusString)
                opacity: 0.75

            }
            SofaWindowComponentMessages { id: windowMessage }

            IconButton {
                /// This is the error button that shows when there is an error message on
                /// an object or a node
                id: childError
                hoverEnabled: true
                anchors.verticalCenter: textLoader.verticalCenter
                anchors.right: componentState.left
                height: 12
                width: 12
                enabled: hasMessage || (hasChildMessage && !styleData.isExpanded) || !root.enabled
                visible: hasMessage || (hasChildMessage && !styleData.isExpanded) || !root.enabled
                iconSource: "qrc:/icon/ICON_WARNING.png"
                useHoverOpacity: false
                layer {
                        enabled: true
                        effect: ColorOverlay {
                            color: {
                                if (isNode) {
                                    if (hasChildMessage || !root.enabled)
                                        return childError.hovered || localError.hovered ? "red" : "darkred"
                                    else
                                        return childError.hovered || localError.hovered ? "#DDDDDD" : "#BBBBBB"
                                } else {
                                    if (hasMessage)
                                        return childError.hovered || localError.hovered ? "red" : "darkred"
                                    else
                                        return childError.hovered || localError.hovered ? "#DDDDDD" : "#BBBBBB"
                                }
                            }
                            onColorChanged: {
                                childError.iconSource = childError.iconSource
                            }
                        }
                }

                onClicked: {
                    if(isNode)
                    {
                        var c = getFirstChildWithMessage(index)
                        var idx = sceneModel.mapFromSource(basemodel.getIndexFromBase(c))
                        treeView.expandAncestors(idx)
                        SofaApplication.selectedComponent = c;
                        treeView.__listView.positionViewAtIndex(index, "EnsureVisible")

                        forceActiveFocus()
                        return
                    }

                    var srcIndex = sceneModel.mapToSource(index)
                    var c = basemodel.getBaseFromIndex(srcIndex)
                    var w = windowMessage.createObject(nodeMenu.parent,{
                                                   "parent" : nodeMenu.parent,
                                                   "sofaComponent": c});
                }
                z: 1
            }





            IconButton {
                /// Window that contains the object message. The windows is only created when the menu item
                /// is clicked

                id: localError
                hoverEnabled: true
                anchors.verticalCenter: textLoader.verticalCenter
                anchors.right: childError.left
                anchors.rightMargin: -6
                height: 12
                width: 12
                visible: (hasMessage || (hasChildMessage && !styleData.isExpanded)) && isNode
                iconSource: "qrc:/icon/ICON_WARNING.png"
                useHoverOpacity: false
                layer {
                        enabled: true
                        effect: ColorOverlay {
                            color: {
                                if (hasMessage)
                                    return childError.hovered || localError.hovered ? "red" : "darkred"
                                else
                                    return childError.hovered || localError.hovered ? "#DDDDDD" : "#BBBBBB"
                            }
                            onColorChanged: {
                                childError.iconSource = childError.iconSource
                            }
                        }
                }
                enabled: hasMessage
                onClicked: {
                    var srcIndex = sceneModel.mapToSource(index)
                    var c = basemodel.getBaseFromIndex(srcIndex)
                    var w = windowMessage.createObject(nodeMenu.parent,{
                                                   "parent" : nodeMenu.parent,
                                                   "sofaComponent": c});

                }
                z: 1
            }

            SofaNodeMenu
            {
                id: nodeMenu
                model: basemodel
                currentModelIndex: sceneModel.mapToSource(styleData.index)
            }

            SofaObjectMenu
            {
                id: objectMenu
                model: basemodel
                currentModelIndex: sceneModel.mapToSource(styleData.index)
            }

            Item {
                id: dragItem
                property string origin: "Hierarchy"
                property SofaBase item
                Drag.active: mouseArea.drag.active
                Drag.onActiveChanged: {
                    var srcIndex = sceneModel.mapToSource(index)
                    var theComponent = basemodel.getBaseFromIndex(srcIndex)
                    item = theComponent
                    print("Dragging " + item.getName())
                }

                Drag.dragType: Drag.Automatic
                Drag.supportedActions: Qt.CopyAction
                Drag.mimeData: {
                    "text/plain": "Copied text"
                }
            }

            ToolTip {
                text: typename
                description: "status: " + statusString
                visible: mouseArea.containsMouse
            }

            MouseArea {
                id: mouseArea
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                anchors.fill: parent
                hoverEnabled: true


                drag.target: dragItem

                Keys.onDeletePressed: {
                    var srcIndex = sceneModel.mapToSource(styleData.index)
                    var parent = basemodel.getBaseFromIndex(srcIndex.parent);
                    var theComponent = basemodel.getBaseFromIndex(srcIndex)
                    if (theComponent.isNode())
                        parent.removeChild(theComponent);
                    else
                        parent.removeObject(theComponent);
                }
                Keys.onPressed: {
                    if (event.key === Qt.Key_F2)
                    {
                        print("renaming....")
                        renaming = true
                    }
                }

                onClicked:
                {
                    forceActiveFocus()
                    var srcIndex = sceneModel.mapToSource(styleData.index)
                    var theComponent = basemodel.getBaseFromIndex(srcIndex)
                    if(mouse.button === Qt.LeftButton) {
                        SofaApplication.selectedComponent = theComponent
                        SofaApplication.currentProject.selectedAsset = null

                        treeView.selection.setCurrentIndex(styleData.index, ItemSelectionModel.ClearAndSelect)
                    } else if (mouse.button === Qt.RightButton) {
                        if(theComponent.isNode()) {
                            //                            nodeMenu.currentModelIndex = srcIndex
                            nodeMenu.activated = theComponent.getData("activated");
                            if(theComponent.hasLocations()===true)
                            {
                                nodeMenu.sourceLocation = theComponent.getSourceLocation()
                                nodeMenu.creationLocation = theComponent.getInstanciationLocation()
                            }
                            nodeMenu.nodeActivated = nodeMenu.activated.value;
                            var pos = SofaApplication.getIdealPopupPos(nodeMenu, mouseArea)
                            nodeMenu.x = mouseArea.mouseX + pos[0]
                            nodeMenu.y = mouseArea.mouseY + pos[1]
                            nodeMenu.open();
                        } else {
                            if(theComponent.hasLocations()===true)
                            {
                                objectMenu.sourceLocation = theComponent.getSourceLocation()
                                objectMenu.creationLocation = theComponent.getInstanciationLocation()
                            }
                            //                            objectMenu.currentModelIndex = srcIndex
                            objectMenu.name = theComponent.getData("name");
                            pos = SofaApplication.getIdealPopupPos(objectMenu, mouseArea)
                            objectMenu.x = mouseArea.mouseX + pos[0]
                            objectMenu.y = mouseArea.mouseY + pos[1]
                            console.error(objectMenu.x + " " + objectMenu.y)
                            objectMenu.open()
                        }
                    }
                }


                DropArea {
                    id: dropArea
                    property SofaBase node: null
                    anchors.fill: parent
                    onEntered: {
                        var srcIndex = sceneModel.mapToSource(styleData.index)
                        var theComponent = basemodel.getBaseFromIndex(srcIndex)
                        if (isNode) node = theComponent
                        else node = theComponent.getFirstParent()
                    }

                    function dropFromHierarchy(src) {
                        var oldIndex = src.index
                        oldIndex = sceneModel.mapToSource(oldIndex)
                        var theComponent = basemodel.getBaseFromIndex(oldIndex)
                        if (!theComponent)
                            return
                        var newIndex = styleData.index
                        newIndex = sceneModel.mapToSource(newIndex)
                        var parentNode = basemodel.getBaseFromIndex(newIndex)

                        if (!parentNode.isNode()) {
                            parentNode = parentNode.getFirstParent()
                        }

                        var oldParent = theComponent.getFirstParent()

                        if (oldParent.getPathName() !== parentNode.getPathName() &&
                                parentNode.getPathName() !== theComponent.getPathName()) {
                            if (theComponent.isNode()) {
                                parentNode.moveChild(theComponent, oldParent)
                            }
                            else {
                                parentNode.moveObject(theComponent)
                            }
                        }
                    }

                    function dropFromProjectView(src) {
                        if (src.asset.typeString === "Python prefab" && src.assetName === "") {
                            var menuComponent = Qt.createComponent("qrc:/SofaWidgets/SofaAssetMenu.qml")
                            var assetMenu = menuComponent.createObject(dropArea, {
                                                                           "asset": src.asset,
                                                                           "parentNode": node,
                                                                           "basemodel": basemodel,
                                                                           "sceneModel": sceneModel,
                                                                           "treeView": treeView,
                                                                           "selection": ItemSelectionModel.ClearAndSelect,
                                                                           "showLoadScene": true
                                                                       });
                            assetMenu.open()
                        }
                        else {
                            var assetNode = src.asset.create(node, src.assetName)
                            if (!assetNode)
                                return
                            var srcIndex = basemodel.getIndexFromBase(assetNode)
                            var index = sceneModel.mapFromSource(srcIndex)
                            treeView.collapseAncestors(index)
                            treeView.expandAncestors(index)
                            treeView.expand(index)
                            treeView.__listView.currentIndex = index.row
                            treeView.selection.setCurrentIndex(index, selectionModel.Select)
                            SofaApplication.selectedComponent = assetNode
                        }
                    }

                    onDropped: {
                        if (drag.source.origin === "Hierarchy") {
                            dropFromHierarchy(drag.source)
                        }
                        else {
                            dropFromProjectView(drag.source)
                        }
                    }
                }
            }
        }

        QQC1.TableViewColumn {
            title: (String(SofaApplication.sofaScene.source).slice(String(SofaApplication.sofaScene.source).lastIndexOf("/")+1))
            role: "name"
        }
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
        anchors.right: root.right
        anchors.rightMargin: 5
        checked: false
        onCheckedChanged: {
            sceneModel.showOnlyNodes(checked)
        }
    }
}

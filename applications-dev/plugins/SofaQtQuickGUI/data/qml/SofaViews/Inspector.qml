/*
Copyright 2016, CNRS

Author: damien.marchal@univ-lille1.fr

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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaComponent 1.0
import SofaInspectorDataListModel 1.0
import SofaWidgets 1.0


/*
  |--------------------------------------------------------|
  | Inspector                                   Debug []   |
  |--------------------------------------------------------|
  | Group 1                                                |
  |   Data 1:                                              |
  |   Data 2:                                              |
  |   Data ...:                                            |
  |--------------------------------------------------------|
  | Group 2                                                |
  |   Data 1:                                              |
  |   Data 2:                                              |
  |   Data ...:                                            |
  |--------------------------------------------------------|

  One ListView is used to render
      Inspector
        Group 1
        Group 2
        ...
  While each group have its own ListView.
*/
Item {
    id: root
    property var selectedAsset: SofaApplication.currentProject.selectedAsset

    Rectangle {
        id: topRect
        color: SofaApplication.style.contentBackgroundColor
        clip: true
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        property var sofaScene: SofaApplication.sofaScene
        property var sofaSelectedComponent: sofaScene ? sofaScene.selectedComponent : null

        property bool showDebug : isDebug.checked
        property int refreshCounter : 0

        visible: selectedAsset === null
        Connections {
            target: sofaScene
            onStepEnd: topRect.refreshCounter = topRect.refreshCounter + 1;
        }

        Timer
        {
            id:updateTimer
            interval: 100
            repeat: true
            onTriggered:
            {
                topRect.refreshCounter = topRect.refreshCounter + 1
            }
        }

        Component.onCompleted:
        {
            if (!topRect.sofaScene)
                topRect.sofaScene = SofaApplication.sofaScene
            topRect.sofaSelectedComponent = topRect.sofaScene ? topRect.sofaScene.selectedComponent : null
        }

        /*
          Each group is composed of an header bar with the group name.
          When clicked the bar collapsed or expand the content of the group.
        */
        VisualDataModel {
            id: visualModel
            model : theModel
            property var currentChild
            property SofaInspectorDataListModel theModel : SofaInspectorDataListModel {
                id : sofaInspectorDataListModel
                currentSofaComponent: theView.sofaSelectedComponent
            }


            delegate: Component {
                id: delegateView
                Rectangle{
                    id: theItem
                    property int groupIndex: index

                    state: sofaInspectorDataListModel.isGroupVisible(index) ? "expanded" : "collapsed"
                    width: parent.width
                    color: SofaApplication.style.contentBackgroundColor
                    clip: true

                    Component.onCompleted : {
                        state =  sofaInspectorDataListModel.isGroupVisible(index) ? "expanded" : "collapsed"
                    }

                    states: [
                        State {
                            name: "expanded"
                            PropertyChanges {
                                target: theItem
                                implicitHeight: childView.childHeight + 26
                            }
                        },
                        State {
                            name: "collapsed"
                            PropertyChanges {
                                target: theItem
                                implicitHeight: 26
                            }
                        }
                    ]

                    /*
                |--------------------------------------------------------|
                | Group 1 (header)                                       |
                |   Data 1:                                              |
                |   Data 2:                                              |
                |   Data ...:                                            |
                |--------------------------------------------------------|*/
                    Column{
                        id : group
                        anchors.fill: parent
                        property int theIndex: index
                        property string theName : name
                        Rectangle{

                            width: theView.width
                            height: 26
                            color: SofaApplication.style.contentBackgroundColor
                            Rectangle {
                                width: theView.width - 20
                                height: 1
                                color: "#393939"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            Rectangle {
                                y: 1
                                width: theView.width - 20
                                height: 1
                                color: "#959595"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            Row{
                                y: 2
                                width: parent.width
                                height: parent.height
                                ColorImage {
                                    id: groupBoxArrow
                                    y: 4
                                    source: theItem.state ==="collapsed" ? "qrc:/icon/rightArrow.png" : "qrc:/icon/downArrow.png"
                                    width: 14
                                    height: 14
                                    color: "#393939"
                                    MouseArea {
                                        id: mouse_area2
                                        anchors.fill: parent
                                        onClicked: {
                                            if(theItem.state === "expanded")
                                                theItem.state = "collapsed"
                                            else
                                                theItem.state = "expanded"

                                            visualModel.model.setVisibility(index, !(theItem.state==="collapsed"))
                                        }
                                    }
                                }

                                Label{
                                    id: titleText
                                    verticalAlignment: Text.AlignVCenter
                                    width: theView.width-20
                                    height:20
                                    text: name
                                    color: "black"
                                    MouseArea {
                                        id: mouse_area1
                                        z: 1
                                        hoverEnabled: false
                                        height : parent.height
                                        width : parent.width - 20
                                        onClicked: {
                                            if(theItem.state === "expanded")
                                                theItem.state = "collapsed"
                                            else
                                                theItem.state = "expanded"

                                            visualModel.model.setVisibility(index, !(theItem.state==="collapsed"))
                                        }
                                    }
                                }

                            }
                        }

                        ListView{
                            property int childHeight: contentHeight
                            property int nameLabelWidth : 100
                            property int nameLabelImplicitWidth : 100
                            id: childView
                            visible: theItem.state != "collapsed"

                            width: theView.width
                            implicitHeight: 2000
                            clip: false

                            Component.onCompleted: {
                                implicitHeight: contentHeight
                                clip = true
                            }

                            model : visualModel

                            property var visualModel : VisualDataModel {
                                property int parentIndex:
                                {
                                    theItem.groupIndex
                                }

                                id : childModel
                                model : visualModel.theModel
                                rootIndex : visualModel.modelIndex(theItem.groupIndex)

                                delegate : visibleItem ;
                                property Component visibleItem :
                                    Column {
                                    Loader{
                                        id: dataLoader
                                        width: theItem.width

                                        sourceComponent: {
                                            if(isReadOnly && !topRect.showDebug)
                                                return hiddenItem;
                                            switch(type){
                                            case 5: //sofaInspectorDataListModel.SofaDataType:
                                                return dataItem;
                                            case 6: //sofaInspectorDataListModel.SofaLinkType:
                                                return linkItem;
                                            case 7: //sofaInspectorDataListModel.InfoItem:
                                                return infoItem;
                                            case 8: //sofaInspectorDataListModel.LogType:
                                                return logItem;
                                            }
                                            return hiddenItem;
                                        }

                                        property Component hiddenItem : Rectangle
                                        {
                                            width : theItem.width ;
                                            height: 0;
                                            color : "yellow"
                                        }

                                        property Component dataItem : SofaDataItem
                                        {

                                            id: sofaDataItem
                                            implicitWidth : theItem.width
                                            implicitHeight: 20

                                            sofaData: getObject(sofaInspectorDataListModel.getDataById(childModel.parentIndex, index))
                                            refreshCounter: topRect.refreshCounter

                                            nameLabelWidth:200

                                            property var cachedObject : null;
                                            function getObject(d){
                                                if(d!==null)
                                                    cachedObject=d;
                                                return cachedObject;
                                            }


                                            Component.onCompleted: updateNameLabelWidth();
                                            onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                                            function updateNameLabelWidth() {
                                                childView.nameLabelImplicitWidth = Math.max(childView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                                            }
                                        }

                                        property Component linkItem: RowLayout {
                                            id: linkView
                                            spacing: 1

                                            Text {
                                                id: linkNameLabel
                                                Layout.preferredWidth: 100
                                                Layout.fillHeight: true
                                                Layout.alignment: Qt.AlignTop

                                                text: name
                                                font.italic: true
                                                verticalAlignment: Text.AlignVCenter;
                                            }
                                            TextField {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                                readOnly: true

                                                text: value.getLinkedPath().trim()
                                                onTextChanged: cursorPosition = 0;

                                                DropArea {
                                                    id: dropArea;
                                                    anchors.fill: parent;
                                                    onEntered: function(drag)
                                                    {
                                                        console.log("DRAG DROPPP");
                                                        if (!isReadOnly && drag.source && drag.source.sofacomponent)
                                                        {
                                                            var sofalink = SofaApplication.sofaScene.link(path)
                                                            sofalink.setValue(drag.source.sofacomponent.getPathName())

                                                            parent.background.border.color = "red";
                                                            drag.accept (Qt.CopyAction);
                                                        }
                                                    }
                                                    onDropped: {
                                                    }
                                                    onExited: {
                                                        parent.background.border.color = "back";
                                                    }
                                                }

                                            }

                                            property int nameLabelWidth: childView.nameLabelImplicitWidth
                                            readonly property int nameLabelImplicitWidth: linkView.implicitWidth

                                            Component.onCompleted: updateNameLabelWidth();
                                            onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                                            function updateNameLabelWidth() {
                                                childView.nameLabelImplicitWidth = Math.max(childView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                                            }
                                        }

                                        property Component infoItem: RowLayout {
                                            spacing: 1

                                            Text {
                                                id: infoNameLabel
                                                Layout.preferredWidth: -1 === nameLabelWidth ? implicitWidth : nameLabelWidth
                                                Layout.fillHeight: true
                                                Layout.alignment: Qt.AlignTop

                                                text: name
                                                font.italic: true
                                            }
                                            Text {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                                Layout.preferredHeight: implicitHeight

                                                text: value.toString().trim()
                                                wrapMode: Text.WordWrap
                                                onTextChanged: {
                                                }
                                            }

                                            property int nameLabelWidth: childView.nameLabelImplicitWidth
                                            readonly property int nameLabelImplicitWidth: infoNameLabel.implicitWidth

                                            Component.onCompleted: updateNameLabelWidth();
                                            onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                                            function updateNameLabelWidth() {
                                                childView.nameLabelImplicitWidth = Math.max(childView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                                            }
                                        }

                                        property Component logItem: GridLayout {
                                            columnSpacing: 1
                                            rowSpacing: 1
                                            columns: 2

                                            Text {
                                                id: logNameLabel
                                                Layout.preferredWidth: -1 === nameLabelWidth ? implicitWidth : nameLabelWidth
                                                Layout.fillHeight: true
                                                Layout.alignment: Qt.AlignTop

                                                text: name
                                                font.italic: true

                                                MouseArea {
                                                    anchors.fill: parent
                                                    onDoubleClicked: sofaDataWindowComponent.createObject(SofaApplication, {"sofaScene": root.sofaScene, "sofaComponent": root.sofaComponent});
                                                }
                                            }
                                            TextArea {
                                                id: logTextArea
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                                Layout.minimumHeight: Math.min(implicitHeight, 400)

                                                text: value.toString().trim()
                                                onTextChanged: cursorPosition = 0;

                                                wrapMode: Text.WordWrap
                                                readOnly: true
                                            }
                                            Button {
                                                Layout.columnSpan: 2
                                                Layout.fillWidth: true

                                                text: "Clear"
                                                onClicked: {
                                                    if(name === "output") root.sofaComponent.clearOutput();
                                                    if(name === "warning") root.sofaComponent.clearWarning();
                                                    logTextArea.text = ""
                                                }
                                            }

                                            property int nameLabelWidth: childView.nameLabelImplicitWidth
                                            readonly property int nameLabelImplicitWidth: logNameLabel.implicitWidth

                                            Component.onCompleted: updateNameLabelWidth();
                                            onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                                            function updateNameLabelWidth() {
                                                childView.nameLabelImplicitWidth = Math.max(childView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


        Rectangle
        {
            id: header
            width: parent.width
            height: 20

            /// This is the header of the inspector.
            Rectangle{
                id: header1
                anchors.left: header.left
                anchors.top: header.top
                width : header.width
                height : 20
                color: SofaApplication.style.contentBackgroundColor

                Text{
                    id: detailsArea
                    anchors.top : header1.top
                    anchors.left : header1.left
                    anchors.verticalCenter: header1.verticalCenter
                    text : "Details " + ((topRect.sofaSelectedComponent===null)? "" : "("+ topRect.sofaSelectedComponent.getClassName() + ")")
                    //                    font.pixelSize: 14
                    //                    font.bold: true
                    color: "black"
                }
                Label {
                    id: showAllLabel
                    anchors.right: isDebug.left
                    anchors.verticalCenter: header1.verticalCenter
                    text: "Show all: "
                    color: "black"
                }
                CheckBox {
                    id : isDebug
                    anchors.right: header1.right
                    anchors.verticalCenter: header1.verticalCenter
                }
            }
        }

        ScrollView {
            anchors.top: header.bottom
            width: parent.width
            height: parent.height - 42
            // TODO(dmarchal): fix the following constant.

            ListView {
                id : theView
                anchors.fill: parent
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                clip: true
                property var sofaSelectedComponent: sofaScene.selectedComponent

                model : visualModel
            }
        }
    }
    Rectangle {
        id: assetArea
        width: parent.width
        height: parent.height - 42
        color: "transparent"
        visible: selectedAsset !== null

        Loader {
            id: assetLoaderId
            source: root.selectedAsset ? root.selectedAsset.getAssetInspectorWidget() : ""
            onLoaded: {
                if (item) {
                    console.error(item)
                    item.selectedAsset = Qt.binding(function() {
                        if (root.selectedAsset)
                            return root.selectedAsset
                        else return null
                    })
                    item.parent = assetArea
                }
            }
        }
    }
}

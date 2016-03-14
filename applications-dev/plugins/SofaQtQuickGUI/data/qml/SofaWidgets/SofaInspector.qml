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

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaComponent 1.0
import SofaInspectorDataListModel 1.0

/*
  |--------------------------------------------------------|
  | Inspector                                              |
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
Rectangle{
    clip: true
    anchors.fill: parent
    property var sofaScene: SofaApplication.sofaScene
    property var sofaSelectedComponent: sofaScene ? sofaScene.selectedComponent : null

    /*
      Each group is composed of an header bar with the group name.
      When clicked the bar collapsed or expand the content of the group.
     */
    VisualDataModel {
        id: visualModel
        model: theModel

        property var currentChild
        property var theModel : SofaInspectorDataListModel {
            id : sofaInspectorDataListModel
            sofaSelectedComponent: theView.sofaSelectedComponent
        }

        delegate: Component {
            Rectangle{
                property var theDataList : null

                id: theItem
                state: "expanded"
                width: parent.width
                color: "lightgrey"
                clip: true

                Component.onCompleted : {
                    state =  sofaInspectorDataListModel.isGroupVisible(index) ? "expanded" : "collapsed"
                }

                Behavior on implicitHeight {
                    NumberAnimation {
                        easing.type: Easing.InOutCubic
                        easing.amplitude: 1.0;
                        easing.period: 1.5
                    }
                }

                states: [
                    State {
                        name: "expanded"
                        PropertyChanges {
                            target: theItem
                            implicitHeight: childView.childHeight + 20
                        }
                    },
                    State {
                        name: "collapsed"
                        PropertyChanges {
                            target: theItem
                            implicitHeight: 20
                        }
                    }
                ]

                Column{
                    anchors.fill: parent

                    property var childModel : visualModel.modelIndex(index) ;

                    Rectangle{
                        width: theView.width
                        height:20
                        color: "grey"
                        radius: 10

                        Text{
                            id: titleText
                            horizontalAlignment: Text.AlignHCenter
                            width: theView.width
                            height:20
                            color: "darkblue"

                            font.bold: true
                            font.pixelSize: 14

                            text: name

                            MouseArea {
                                id: mouse_area1
                                z: 1
                                hoverEnabled: false
                                height : parent.height
                                width : parent.width
                                onClicked: {
                                    if(theItem.state == "expanded")
                                        theItem.state = "collapsed"
                                    else
                                        theItem.state = "expanded"
                                }
                            }
                        }
                    }

                    ListView{
                        property int childHeight : contentHeight
                        id: childView
                        visible: theItem.state == "expanded"
                        width: theView.width
                        height: contentHeight
                        clip: true

                        model : VisualDataModel {
                            id : childModel
                            model : visualModel.theModel
                            rootIndex : visualModel.modelIndex(index)
                            property var parentIndex: index

                            delegate : visibleItem ;


                            property Component hiddenItem : Rectangle {
                                width : theItem.width ;
                                height: 100 ;
                                color : "yellow"
                            }
                            property Component visibleItem :
                                Column {
                                Loader{
                                    id: dataLoader
                                    width: theItem.width
                                    sourceComponent: {
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
                                    }

                                    property Component dataItem : SofaDataItem {
                                        implicitWidth :  theItem.width
                                        property int nameLabelWidth: 100
                                        id: sofaDataItem

                                        sofaScene: root.sofaScene
                                        sofaData: sofaInspectorDataListModel.getDataById(childModel.parentIndex, index)
                                    }

                                    property Component linkItem: RowLayout {
                                        spacing: 1
                                        property int nameLabelWidth: 100

                                        Text {
                                            id: linkNameLabel
                                            Layout.preferredWidth: 100
                                            Layout.fillHeight: true
                                            Layout.alignment: Qt.AlignTop

                                            text: name
                                            font.italic: true
                                            verticalAlignment: Text.AlignVCenter;

                                            MouseArea {
                                                anchors.fill: parent
                                                onDoubleClicked: sofaDataWindowComponent.createObject(SofaApplication, {"sofaScene": root.sofaScene, "sofaComponent": root.sofaComponent});
                                            }
                                        }
                                        TextField {
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            Layout.preferredHeight: implicitHeight
                                            readOnly: true

                                            text: value.toString().trim()
                                            onTextChanged: cursorPosition = 0;
                                        }
                                    }

                                    property Component infoItem: RowLayout {
                                        spacing: 1
                                        property int nameLabelWidth: 100

                                        Text {
                                            id: infoNameLabel
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
                                        Text {
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            Layout.preferredHeight: implicitHeight

                                            text: value.toString().trim()
                                            wrapMode: Text.WordWrap
                                        }

                                        /*property int nameLabelWidth: listView.nameLabelImplicitWidth
                                        readonly property int nameLabelImplicitWidth: infoNameLabel.implicitWidth

                                        Component.onCompleted: updateNameLabelWidth();
                                        onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                                        function updateNameLabelWidth() {
                                            listView.nameLabelImplicitWidth = Math.max(listView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                                        }*/
                                    }

                                    property Component logItem: GridLayout {
                                        columnSpacing: 1
                                        rowSpacing: 1
                                        columns: 2
                                        property int nameLabelWidth: 100


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

                                        //property int nameLabelWidth: listView.nameLabelImplicitWidth
                                        //readonly property int nameLabelImplicitWidth: logNameLabel.implicitWidth

                                        //Component.onCompleted: updateNameLabelWidth();
                                        //onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                                        //function updateNameLabelWidth() {
                                        //    listView.nameLabelImplicitWidth = Math.max(listView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                                        //}
                                    }
                                }

                                Rectangle{
                                    width : theView.width
                                    height: 1
                                    color: "grey"
                                }
                                // }
                            }
                        }
                    }
                }
            }
        }
    }

    Column{
        width: parent.width
        height: parent.height
        Rectangle{
            width: parent.width
            height: 20
            color: "darkgrey"
            Text{
                text : "Details " + ((sofaSelectedComponent===null)? "" : "("+ sofaSelectedComponent.className() + ")")

                font.pixelSize: 14
                font.bold: true
            }
        }
        Rectangle{
            width: parent.width
            height: 25
            color: "darkgrey"

            Row{

                Text{
                    id: nameText
                    height: 20
                    text : "name:"
                    font.pixelSize: 12
                    verticalAlignment: Text.AlignVCenter
                }
                TextField {
                    implicitWidth: 200
                    implicitHeight: 22
                    id: nameTextField
                    text: (sofaSelectedComponent===null)? "" : sofaSelectedComponent.name()

                    style: TextFieldStyle {
                        background: Rectangle {
                            radius: 2
                            color: "lightgray"
                            border.color: "grey"
                            border.width: 0
                        }
                        font.pixelSize: 12

                    }
                    verticalAlignment: Text.AlignVCenter;

                    onEditingFinished : {
                        sofaSelectedComponent.beginDataChange() ;
                        sofaSelectedComponent.setName(text)
                        sofaSelectedComponent.endDataChange() ;
                    }
                }
            }
        }


        ScrollView {
            Layout.fillWidth: true
            width: parent.width
            height: parent.height

            ListView {
                id : theView
                anchors.fill: parent
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                clip: true
                property var sofaScene: SofaApplication.sofaScene
                property var sofaSelectedComponent: sofaScene ? sofaScene.selectedComponent : null

                model : visualModel
            }
        }
    }
}

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

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import SofaBasics 1.0
import SofaApplication 1.0
import QtGraphicalEffects 1.12

Column {
    property bool filterByComponent : false
    property var sofaScene: SofaApplication.sofaScene
    property var selectedComponent: SofaApplication.selectedComponent
    property alias showEmittingLocation: buttonShowEmittingLocation.checked
    property string selectedComponentPath : selectedComponent ? selectedComponent.getPathName() : ""

    /// We want to reset the counter of selected elements. I'm not sure how to
    /// do that in a declarative way through properties.
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true
    anchors.fill: parent

    Rectangle{
        id: header
        width: parent.width
        height: 20
        color: SofaApplication.style.contentBackgroundColor

        Text{
            id: hname
            anchors.left: header.left
            anchors.top: header.top
            anchors.bottom: header.bottom
            font.pixelSize: 12
            font.bold: true
            text : "Messages (" + SofaApplication.sofaMessageList.messageCount + ") "
        }
        IconButton {
            id: buttonClearHistory
            width: 14
            height: 14
            anchors.left: hname.right
            anchors.verticalCenter: parent.verticalCenter
            iconSource: "qrc:/icon/clear.png"
            onClicked: SofaApplication.sofaMessageList.clear();
        }

        ComboBox {
            anchors.left: buttonClearHistory.right
            anchors.leftMargin: 4
            anchors.top: header.top
            anchors.bottom: header.bottom
            width: 150
            currentIndex: filterByComponent? 1 : 0
            model: [ "All", "SelectedComponent" ]
            onActivated: {
                filterByComponent = index!==0
            }
        }
        CheckBox {
            id: buttonShowEmittingLocation
            anchors.right: header.right
            anchors.top: header.top
            anchors.bottom: header.bottom
            anchors.leftMargin: 6
            anchors.rightMargin: 6
            checked: false
        }
        Text {
            id: checkBoxLabel
            text : "Show details:"
            anchors.right: buttonShowEmittingLocation.left
            anchors.top: header.top
            anchors.bottom: header.bottom
            anchors.leftMargin: 6
            anchors.rightMargin: 6
        }
    }

    Rectangle {
        id: messagearea
        implicitWidth:  parent.width
        implicitHeight: parent.height
        color : SofaApplication.style.editviewBackgroundColor

        ScrollView {
            id: view

            Layout.fillWidth: true
            width: parent.width
            height: parent.height

            ListView {
                anchors.fill: parent
                id: p1scores
                model: SofaApplication.sofaMessageList
                Layout.fillWidth: true
                clip: true
                focus: true

                Connections {
                    target : SofaApplication.sofaMessageList
                    onRowsRemoved : {
                        if( p1scores.currentIndex <= 0 ){
                            p1scores.currentIndex = -1
                        }
                    }
                }

                delegate: Component {
                    Rectangle{
                        id: viewitem
                        state: {
                            if(filterByComponent)
                                return "s3"
                            return showEmittingLocation ? "s1" : "s2"
                        }
                        width: parent.width;
                        clip : true
                        height: columnA.height

                        /// The message is showned iff the either the message match the emitter's activeFocus:
                        /// or filtering is disabled.
                        visible:
                        {
                            if(filterByComponent && (selectedComponentPath.length != 0))
                            {
                                if(emitterpath === selectedComponentPath){
                                    return true
                                }
                                return false
                            }
                            return true
                        }

                        property alias baseinfo: baseinfo
                        property alias extrainfo: extrainfo

                        Column {
                            id : columnA
                            property alias baseinfo: baseinfo
                            property alias extrainfo: extrainfo
                            Row{
                                id: baseinfo
                                spacing : 10
                                Text{
                                    id: messageemitter
                                    textFormat: Text.RichText
                                    text: {
                                        // Info=0, Advice, Deprecated, Warning, Error, Fatal,
                                        if(type == 0)
                                            return "[<font color='#00ff00'><b>INFO</b></font>] <font color='#6495ed'>[<u>"+emitter+"</u>]</font>"
                                        if(type == 1)
                                            return "[<font color='#00ff00'><b>ADVICE</b></font>] <font color='#6495ed'>[<u>"+emitter+"</u>]</font>"
                                        if(type == 2)
                                            return "[<font color='#ffD700'><b>DEPRECATED</b></font>] <font color='#6495ed'>[<u>"+emitter+"</u>]</font>"
                                        if(type == 3)
                                            return "[<font color='#00FFFF'><b>WARNING</b></font>] <font color='#6495ed'>[<u>"+emitter+"</u>]</font>"
                                        if(type == 4)
                                            return "[<font color='#ff0000'><b>ERROR</b></font>] <font color='#6495ed'>[<u>"+emitter+"</u>]</font>"
                                        if(type == 5)
                                            return "[<font color='#ff0000'><b>FATAL</b></font>] <font color='#6495ed'>[<u>"+emitter+"</u>]</font>"
                                    }
                                    wrapMode: Text.Wrap

                                    /// When we click on the emitter a visual signal is emitted
                                    /// to indicate where is that the "source" of the message
                                    MouseArea {
                                        anchors.fill: parent
                                        visible: emitterpath != null
                                        enabled: emitterpath != null
                                        /// Change the cursor shape to apointing hand
                                        cursorShape: Qt.PointingHandCursor
                                        hoverEnabled: true

                                        onEntered: {
                                            sofaScene.statusMessage = "Left-click to select ["+emitterpath+"]"
                                        }
                                        onExited: { sofaScene.statusMessage = "" }

                                        /// When the emitter is clicked, it signals that the
                                        /// user is interested to locate the 'target' componen.
                                        onClicked: {
                                            SofaApplication.signalComponent(emitterpath)
                                        }
                                    }
                                }
                                Text{
                                    id: messagetext
                                    width: messagearea.implicitWidth - messageemitter.width - 40
                                    textFormat: Text.RichText
                                    wrapMode: Text.Wrap
                                    text: message
                                    color: "black"
                                }
                                IconButton
                                {
                                    id: messageicon
                                    width: 16
                                    height: 16
                                    enabled: link !== "undefined"
                                    anchors.verticalCenter: parent.verticalCenter
                                    iconSource: "qrc:/icon/ICON_LEAVE.png"
                                    useHoverOpacity: false
                                    ColorOverlay {
                                        source: parent
                                        anchors.fill: parent
                                        color: {
                                            if (messageicon.hovered)
                                                return "orange"
                                            else if (link !== "undefined")
                                                return "#EEEEEE"
                                            else
                                                return "gray"
                                        }

                                    }

                                    onClicked: {
                                        SofaApplication.openInEditor(link, line)
                                    }
                                }
                            }

                            Text {
                                property string filename: link
                                property string dline: line
                                id : extrainfo
                                width: messagearea.width
                                elide: Text.ElideRight
                                textFormat: Text.RichText
                                text: "Emitted from: <font color='#aeaeae'>"+link+":"+line+"</font>"
                                color: "black"

                                /// When we click on the emitter a visual signal is emitted
                                /// to indicate where is that the "source" of the message
                                MouseArea {
                                    height : extrainfo.height
                                    width : extrainfo.width

                                    /// Change the cursor shape to apointing hand
                                    cursorShape: Qt.PointingHandCursor
                                    hoverEnabled: true

                                    onEntered: { sofaScene.statusMessage = "Left-click to open this file in an external text editor. "}
                                    onExited: { sofaScene.statusMessage = "" }

                                    /// When the emitter is clicked, it signals that the
                                    /// user is interested to locate the 'target' componen.
                                    onClicked: {
                                        SofaApplication.openInEditor(extrainfo.filename, extrainfo.dline)
                                    }
                                }
                            }
                            Rectangle{
                                id: spacerline0
                                width: p1scores.width
                                height:6
                                color: SofaApplication.style.contentBackgroundColor
                            }
                            Rectangle{
                                id: spacerline1
                                width: p1scores.width
                                height:1
                                color: "#393939"
                            }
                            Rectangle{
                                id: spacerline2
                                width: p1scores.width
                                height:1
                                color: "#959595"
                            }
                        }

                        states: [
                            State {
                                name: "s1"
                                PropertyChanges {
                                    target: viewitem
                                    extrainfo.visible: true
                                    height: columnA.height + 4
                                    color: SofaApplication.style.contentBackgroundColor
                                }
                            },
                            State {
                                name: "s2"
                                PropertyChanges {
                                    target: viewitem
                                    extrainfo.visible: false
                                    height: columnA.height + 4
                                    color: SofaApplication.style.contentBackgroundColor
                                }
                            },
                            State {
                                name: "s3"
                                PropertyChanges {
                                    target: viewitem
                                    extrainfo.visible: showEmittingLocation
                                    height: viewitem.visible ? columnA.height + 4 : 0
                                    color: SofaApplication.style.contentBackgroundColor
                                }
                            }
                        ]
                    }
                }
                highlightFollowsCurrentItem: true
            }
        }
    }
}

/*
Copyright 2016,
Author: damien.marchal@univ-lille1.fr, Copyright 2016 CNRS.

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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import SofaBasics 1.0
import SofaApplication 1.0
import SofaScene 1.0
import SofaMessageList 1.0

Column {
    property bool filterByComponent : false

    property var sofaScene: SofaApplication.sofaScene
    property var sofaSelectedComponent: sofaScene.selectedComponent
    property string selectedComponentPath : sofaSelectedComponent ? sofaSelectedComponent.getPathName() : ""

    /// We want to reset the counter of selected elements. I'm not sure how to
    /// do that in a declarative way through properties.
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true
    anchors.fill: parent

    Rectangle{
        id: header
        width: parent.width
        height: headerLayout.height
        color: "darkgrey"
        Row{
            id: headerLayout
            spacing: 10
            Text{
                id: hname
                text : "Messages (" + SofaMessageList.messageCount + ")"
                font.pixelSize: 12
                font.bold: true
            }
            IconButton {
                id: buttonClearHistory
                height:12
                iconSource: "qrc:/icon/invalid.png"

                onClicked: {
                    SofaMessageList.clear();
                }
            }
            ComboBox {
                width: 150
                height:16
                style: ComboBoxStyle {
                    font.pixelSize: 12
                }
                model: [ "All", "SelectedComponents" ]
                onActivated: {
                    filterByComponent = index!==0
                }
            }

        }
    }

    Rectangle {
        id: messagearea
        implicitWidth:  parent.width
        implicitHeight: parent.height
        color : "lightgrey"


        ScrollView {
            id: view

            Layout.fillWidth: true
            width: parent.width
            height: parent.height

            ListView {
                anchors.fill: parent
                id: p1scores
                model: SofaMessageList
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                clip: true
                focus: true

                Connections {
                    target : SofaMessageList
                    onRowsRemoved : {
                        if( p1scores.currentIndex <= 0 ){
                            p1scores.currentIndex = -1
                        }
                    }
                }

                delegate: Component {
                    Rectangle{
                        id: viewitem
                        state: "s1"
                        width: parent.width;
                        clip : true

                        /// The message is showned iff the either the message match the emitter's activeFocus:
                        /// or filtering is disabled.
                        visible: {
                            if(filterByComponent && (selectedComponentPath.length != 0))
                            {
                                if(emitterpath === selectedComponentPath){
                                    return true
                                }
                                return false
                            }
                            return true
                        }

                        Behavior on height {
                            NumberAnimation {
                                easing.type: Easing.InOutCubic
                                easing.period: 1.5
                            }
                        }

                        onChildrenRectChanged: {
                            if(viewitem.state=="s1")
                                height=baseinfo.height+extrainfo.height+3
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
                                height: messagetext.height

                                Text{
                                    id: messagetype
                                    textFormat: Text.RichText
                                    text: {
                                        // Info=0, Advice, Deprecated, Warning, Error, Fatal,
                                        if(type == 0)
                                            return "[<font color='#00ff00'>INFO</font>]: "+emitter
                                        if(type == 1)
                                            return "[<font color='#00ff00'>ADVICE</font>]: "+emitter
                                        if(type == 2)
                                            return "[<font color='#ff0000'>DEPRECATED</font>]: "+emitter
                                        if(type == 3)
                                            return "[<font color='#998800'>WARNING</font>]: "+emitter
                                        if(type == 4)
                                            return "[<font color='#ff0000'>ERROR</font>]: "+emitter
                                        if(type == 5)
                                            return "[<font color='#ff0000'>FATAL</font>]: "+emitter
                                    }

                                    /// When we click on the emitter a visual signal is emitted
                                    /// to indicate where is that the "source" of the message
                                    MouseArea {
                                        height : messagetype.height
                                        width : messagetype.width

                                        /// Change the cursor shape to apointing hand
                                        cursorShape: Qt.PointingHandCursor
                                        hoverEnabled: false

                                        /// When the emitter is clicked, it signals that the
                                        /// user is interested to locate the 'target' componen.
                                        onClicked: {
                                            SofaApplication.signalComponent(emitterpath)
                                        }
                                    }
                                }
                                Text{
                                    id: messagetext
                                    width: messagearea.width - messagetype.width-10
                                    textFormat: Text.RichText
                                    wrapMode: Text.Wrap
                                    text: message
                                }
                            }
                            Text{
                                id : extrainfo
                                width: messagearea.width
                                wrapMode: Text.WrapAnywhere
                                textFormat: Text.RichText
                                text: "Emitted from: <font color='#998800'>"+link+":"+line+"</font>"

                                /// When we click on the emitter a visual signal is emitted
                                /// to indicate where is that the "source" of the message
                                MouseArea {
                                    height : extrainfo.height
                                    width : extrainfo.width

                                    /// Change the cursor shape to apointing hand
                                    cursorShape: Qt.PointingHandCursor
                                    hoverEnabled: false

                                    /// When the emitter is clicked, it signals that the
                                    /// user is interested to locate the 'target' componen.
                                    onClicked: {
                                        SofaApplication.openInEditor(link, line)
                                    }
                                }
                            }
                            Rectangle{
                                id: endline
                                width: p1scores.width
                                height: 1
                                color: "gray"
                            }
                            Rectangle{
                                id: spacerline
                                width: p1scores.width
                                height:3
                                color: Qt.rgba(0.85, 0.85, 0.85, 1.0)
                            }
                        }

                        states: [
                            State {
                                name: "hidden"
                                PropertyChanges {
                                    target: viewitem
                                    visible: false
                                    height: childrenRect.height

                                    color: "lightsteelblue"
                                }
                            },
                            State {
                                name: "s1"
                                PropertyChanges {
                                    target: viewitem
                                    extrainfo.visible: true
                                    height: childrenRect.height

                                    color: "lightsteelblue"
                                }
                            },
                            State {
                                name: "s2"
                                PropertyChanges {
                                    target: viewitem
                                    baseinfo.height: 15
                                    extrainfo.visible: false
                                    //height: 18
                                    //height: root.filterByComponent && root.sofaSelectedComponent !== emitter? 0 : childrenRect.height

                                    color: Qt.rgba(0.85, 0.85, 0.85, 1.0)
                                }
                            }
                        ]

                        //                        MouseArea {
                        //                            id: mouse_area1
                        //                            z: 1
                        //                            hoverEnabled: false
                        //                            height : viewitem.height
                        //                            width : parent.width
                        //                            onClicked: {
                        //                                if(index == p1scores.currentIndex)
                        //                                    p1scores.currentIndex = -1 ;
                        //                                else
                        //                                    p1scores.currentIndex = index ;
                        //                            }
                        //                        }

                    }
                }
                highlightFollowsCurrentItem: true
            }
        }
    }
}

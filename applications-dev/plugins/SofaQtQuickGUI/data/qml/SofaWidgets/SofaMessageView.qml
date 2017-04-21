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
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import SofaBasics 1.0
import SofaApplication 1.0
import SofaScene 1.0
import SofaMessageList 1.0

Column {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true
    anchors.fill: parent
    property var sofaScene: SofaApplication.sofaScene

    Rectangle{
            id: header
            width: parent.width
            height: 16
            color: "darkgrey"
            Row{
                Text{
                    id: hname
                    text : "Messages (" + SofaMessageList.messageCount + ")"
                    font.pixelSize: 12
                    font.bold: true
                }

                IconButton {
                    id: openSourceFile
                    height:12
                    anchors.horizontalCenter: hname.anchors.horizontalCenter
                    iconSource: "qrc:/icon/invalid.png"

                    onClicked: {
                        SofaMessageList.clear();
                    }
                }

            }
    }


    Rectangle {
        implicitWidth: parent.width
        implicitHeight: parent.height - header.height
        color : "lightgrey"

        ScrollView {
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
                        state: index == p1scores.currentIndex ? "s1" : "s2"

                        width: parent.width;
                        clip : true

                        Behavior on height {
                            NumberAnimation {
                                easing.type: Easing.InOutCubic
                                easing.period: 1.5
                            }
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
                                }
                                Text{
                                    width : 800
                                    textFormat: Text.RichText
                                    wrapMode: Text.Wrap
                                    text: message
                                }
                            }
                            Text{
                                id : extrainfo

                                textFormat: Text.RichText
                                text: "Emitted from: <a href='file://"+link+"?"+line+"'>"+link+":"+line+"</a>"
                                onLinkActivated: {
                                    Qt.openUrlExternally(link);
                                }
                            }
                            Rectangle{
                                id: endline
                                width: p1scores.width
                                height: 1
                                color: "gray"
                            }
                            Rectangle{
                                id: endline2
                                width: p1scores.width
                                height: 1
                                color: Qt.rgba(0.85, 0.85, 0.85, 1.0)
                            }
                        }

                        states: [
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
                                    height: 18
                                    color: Qt.rgba(0.85, 0.85, 0.85, 1.0)
                                }
                            }
                        ]

                        MouseArea {
                            id: mouse_area1
                            z: 1
                            hoverEnabled: false
                            height : {
                                if(index == p1scores.currentIndex)
                                    return viewitem.childrenRect.implicitHeight - 15
                                else
                                    return 20
                            }
                            width : parent.width
                            onClicked: {
                                if(index == p1scores.currentIndex)
                                    p1scores.currentIndex = -1 ;
                                else
                                    p1scores.currentIndex = index ;
                            }
                        }

                    }
                }
                highlightFollowsCurrentItem: true
            }
        }
    }
}

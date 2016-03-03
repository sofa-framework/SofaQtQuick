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
import Console 1.0

ColumnLayout {
    id: root
    spacing: 0

    property var sofaScene: SofaApplication.sofaScene

    Rectangle {
        anchors.fill: parent
        Layout.fillWidth: true
        Layout.fillHeight: true

        ListModel {
            id: p1model
        }

        ScrollView {
            Layout.fillWidth: true
            width: parent.width
            height: parent.height

            ListView {
                anchors.fill: parent
                id: p1scores
                model: p1model
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                clip: true
                focus: true

                delegate: Component {
                    Rectangle{
                        id: viewitem
                        state: index == p1scores.currentIndex ? "s1" : "s2"

                        width: parent.width;
                        clip : true

                        Behavior on height {
                            SpringAnimation {
                                spring: 3
                                damping: 0.2
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
                                        if(type == 0)
                                            return "[<font color='#00ff00'>INFO</font>]: "+emitter
                                        if(type == 1)
                                            return "[<font color='#998800'>WARNING</font>]: "+emitter
                                        if(type == 2)
                                            return "[<font color='#ff0000'>ERROR</font>]: "+emitter
                                        return "[<font color='#ff0000'>DEPRECATED</font>]: "+emitter
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
                                    console.log("Helle est pas belle la vie ? "+link);
                                }
                                /*MouseArea {
                                        anchors.fill: parent
                                        cursorShape: text.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                                        acceptedButtons: Qt.NoButton
                                }*/
                                /*onLinkHovered: {
                                    console.log("TOTO !!" +link) ;
                                }*/
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
                                    return viewitem.childrenRect.height - 15
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


        Console {
            onMessageAdded:{
                if(p1model.count > 100){
                    p1model.remove(0);
                    if( p1scores.currentIndex <= 0 )
                        p1scores.currentIndex = -1;
                    else
                        p1scores.currentIndex = p1scores.currentIndex-1;
                }
                p1model.append({emitter : emitter,
                                   type : type,
                                   message : message,
                                   link : source,
                                   line : line});

            }
        }
    }
}

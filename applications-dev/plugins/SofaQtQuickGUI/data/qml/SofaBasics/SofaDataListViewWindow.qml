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
import SofaComponent 1.0
import SofaDataListModel 1.0

Window {
    id: root
    width: 400
    height: Math.min(600, listView.contentHeight)
    modality: Qt.NonModal
    flags: Qt.Dialog | Qt.WindowStaysOnTopHint
    visible: true
    color: "lightgrey"

    title: sofaComponent ? ("Data of component: " + sofaComponent.name()) : "No component to visualize"

    property var sofaScene: SofaApplication.sofaScene
    property var sofaComponent: sofaScene ? sofaScene.selectedComponent : null

    ScrollView {
        id: scrollView
        anchors.fill: parent
        enabled: sofaScene && sofaComponent ? sofaScene.ready : false
        frameVisible: true

        ListView {
            id: listView
            focus: true
            anchors.fill: parent
            anchors.rightMargin: 8

            property int nameLabelImplicitWidth : 16

            model: SofaDataListModel {
                id: sofaDataListModel
                sofaComponent: root.sofaComponent

                onSofaComponentChanged: listView.nameLabelImplicitWidth = 16;
            }

            section.property: "group"
            section.criteria: ViewSection.FullString
            section.delegate: Rectangle {
                width: parent.width
                height: childrenRect.height
                color: "darkgrey"
                radius: 10

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: section
                    color: "darkblue"
                    font.bold: true
                    font.pixelSize: 14
                }
            }

            delegate: Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                height: sofaDataItem.height
                color: index % 2 ? Qt.rgba(0.85, 0.85, 0.85, 1.0) : Qt.rgba(0.9, 0.9, 0.9, 1.0)

                Rectangle {
                    visible: sofaDataItem.modified
                    anchors.fill: sofaDataItem
                    color: "lightsteelblue"
                    radius: 5
                }

                SofaDataItem {
                    id: sofaDataItem
                    anchors.left: parent.left
                    anchors.right: parent.right

                    sofaScene: root.sofaScene
                    sofaData: sofaDataListModel.getDataById(index)

                    nameLabelWidth: listView.nameLabelImplicitWidth
                    Component.onCompleted: updateNameLabelWidth();
                    onNameLabelImplicitWidthChanged: updateNameLabelWidth();

                    function updateNameLabelWidth() {
                        listView.nameLabelImplicitWidth = Math.max(listView.nameLabelImplicitWidth, nameLabelImplicitWidth);
                    }
                }
            }
        }
    }
}

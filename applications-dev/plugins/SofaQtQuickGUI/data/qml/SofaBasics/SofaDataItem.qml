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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import SofaApplication 1.0
import Sofa.Core.SofaData 1.0
import SofaBasics 1.0

Item {
    id: self

    implicitWidth: gridlayout.implicitWidth
    implicitHeight: dataWidget.implicitHeight

    signal doubleClickedOnLabel;

    property SofaData sofaData: null
    property var widget: null

    property int nameLabelWidth: -1
    readonly property int nameLabelImplicitWidth: nameLabel.implicitWidth

    property bool readOnly: false
    property bool showName: true
    property bool showLinkButton: true
    //property bool showTrackButton: true
    //property alias track: trackButton.checked
    property int refreshCounter: 0

    function updateData(newValue)
    {
        console.log("Update Data " + sofaData.name);
        sofaData.value = newValue;
    }

    function updateLink()
    {
        console.log("Update Link " + sofaData.name + " with value: " + linkTextField.text)

        sofaData.setLink(linkTextField.visible ? linkTextField.text : "");
        linkTextField.borderColor = Qt.binding(function (){return (0 === sofaData.linkPath.length) ? "red" : "#393939"})
        sofaData.value = sofaData.value
    }

    GridLayout {
        id: gridlayout
        anchors.fill: parent
        columns: 4

        columnSpacing: 1
        rowSpacing: 1

        Label {
            id: nameLabel
            Layout.preferredWidth: -1 === nameLabelWidth ? implicitWidth : nameLabelWidth
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            visible: self.showName
            text: sofaData ? sofaData.name : ""
            font.italic: true
            color: "black"
            width: parent.width
            clip: true
            elide: Text.ElideRight

            MouseArea {
                id: dataLabelMouseArea
                hoverEnabled: true
                anchors.fill: parent
                onDoubleClicked: self.doubleClickedOnLabel();
            }
            ToolTip {
                text: sofaData? sofaData.name : ""
                description: sofaData ? sofaData.help : ""
                visible: dataLabelMouseArea.containsMouse
            }

        }

        ColumnLayout
        {
            id: datawidget
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            Layout.minimumWidth: 100

            RowLayout {
                Layout.fillWidth: true
                visible: sofaData && 0 !== sofaData.name.length && (linkButton.checked || (0 !== sofaData.linkPath.length && !self.showLinkButton))
                spacing: 0

                TextField {
                    id: linkTextField
                    Layout.fillWidth: true
                    placeholderText: sofaData ? "Link: @./path/component." + sofaData.name : ""
                    placeholderTextColor: "gray"
                    text: sofaData ? sofaData.linkPath : ""
                    width: parent.width
                    clip: true
                    onEditingFinished: updateLink();
                    borderColor: 0 === sofaData.linkPath.length ? "red" : "#393939"
                }
            }
        }

        Item {
            Layout.preferredWidth: 20
            Layout.preferredHeight: Layout.preferredWidth
            Layout.alignment: Qt.AlignTop
            visible: self.showLinkButton

            Button {
                id: linkButton
                anchors.fill: parent
                anchors.margins: 3
                checkable: true
                checked: sofaData ? 0 !== sofaData.linkPath.length : false
                activeFocusOnTab: false
                ToolTip {
                    text: "Link the data to another one."
                }

                onClicked: {
                    updateLink()
                }
                Image {
                    anchors.fill: parent
                    source: "qrc:/icon/link.png"
                }
            }
        }

//        CheckBox {
//            id: trackButton
//            visible: self.showTrackButton && 0 !== sofaData.name.length
//            checked: false

//            onClicked: self.updateObject();

//            ToolTip {
//                text: "Update the visualization of the data during simulation."
//            }

//            Timer {
//                interval: 50
//                repeat: true
//                running: trackButton.checked

//                onRunningChanged: self.updateObject();
//                onTriggered: self.updateObject();
//            }
//        }
    }

    onSofaDataChanged:
    {
        if(sofaData)
        {
            /// Returns the widget's properties associated with this SofaData
            var component = SofaDataWidgetFactory.getWidgetForData(sofaData)
            var o = component.createObject(datawidget, {"sofaData": sofaData,
                                                "Layout.fillWidth":true})
            self.implicitHeight = o.implicitHeight < 20 ? 20 : o.implicitHeight
            o.visible = Qt.binding(function() { return !linkButton.checked })
        }
    }
}

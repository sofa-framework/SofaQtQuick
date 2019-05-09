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
    id: root

    implicitWidth: gridlayout.implicitWidth
    implicitHeight: gridlayout.implicitHeight

    signal doubleClickedOnLabel;

    property QtObject sofaData: null

    property int nameLabelWidth: -1
    readonly property int nameLabelImplicitWidth: nameLabel.implicitWidth

    readonly property alias name:       dataObject.name
    readonly property alias description:dataObject.description
    readonly property alias type:       dataObject.type
    readonly property alias group:      dataObject.group
    readonly property alias properties: dataObject.properties
    readonly property alias value:      dataObject.value
    readonly property alias modified:   dataObject.modified

    property bool readOnly: false
    property bool showName: true
    property bool showLinkButton: true
    property bool showTrackButton: true
    property alias track: trackButton.checked

    QtObject {
        id: dataObject

        property bool initing: true
        property QtObject sofaData
        property string name
        property string description
        property string type
        property string group
        property var properties
        property string link
        property var value
        property bool modified: false
        property bool readOnly: initing || root.readOnly || properties.readOnly || trackButton.checked || linkButton.checked

        signal updated;

        onValueChanged:
            root.updateData();
    }

    function updateObject()
    {
        if(!sofaData)
            return;

        var object              = sofaData.object();
        dataObject.initing      = true;

        dataObject.sofaData     = object.sofaData;
        dataObject.name         = object.name;
        dataObject.description  = object.description;
        dataObject.type         = object.type;
        dataObject.group        = object.group;
        dataObject.properties   = object.properties;
        dataObject.link         = object.link;
        dataObject.value        = object.value;

        dataObject.initing      = false;
        dataObject.modified     = false;
    }

    function updateData()
    {
        if(!sofaData)
            return;
        console.log("UPDATE DATA for "+sofaData.getName())
        sofaData.setValue(dataObject.value);
    }

    function updateLink() {
        if(!sofaData)
            return;

        sofaData.setLink(linkTextField.visible ? linkTextField.text : "");
        updateObject();
    }

    Rectangle {
        anchors.fill: parent
        visible: root.modified
        color: "lightsteelblue"
        radius: 5
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
            visible: root.showName
            text: dataObject.name
            font.italic: true
            color: "black"
            width: parent.width
            clip: true
            elide: Text.ElideRight

            MouseArea {
                id: dataLabelMouseArea
                hoverEnabled: true
                anchors.fill: parent
                onDoubleClicked: root.doubleClickedOnLabel();
            }
            ToolTip {
                text: dataObject.name
                description: dataObject.description
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
                visible: 0 !== dataObject.name.length && (linkButton.checked || (0 !== dataObject.link.length && !root.showLinkButton))
                spacing: 0

                TextField {
                    id: linkTextField
                    Layout.fillWidth: true
                    placeholderText: "Link: @./path/component." + dataObject.name
                    color: 0 === dataObject.link.length ? "black" : "green"
                    text: dataObject.link
                    width: parent.width
                    clip: true
                    onTextChanged: updateLink();
                }

                Image {
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: Layout.preferredWidth
                    source: 0 === dataObject.link.length ? "qrc:/icon/invalid.png" : "qrc:/icon/correct.png"
                }
            }
        }

        Item {
            Layout.preferredWidth: 20
            Layout.preferredHeight: Layout.preferredWidth
            Layout.alignment: Qt.AlignTop
            visible: root.showLinkButton

            Button {
                id: linkButton
                anchors.fill: parent
                anchors.margins: 3
                checkable: true
                checked: 0 !== dataObject.link.length

                ToolTip {
                    text: "Link the data to another one."
                }

                onClicked: updateLink()

                Image {
                    anchors.fill: parent
                    source: "qrc:/icon/link.png"
                }
            }
        }

        CheckBox {
            id: trackButton
            visible: root.showTrackButton && 0 !== dataObject.name.length
            checked: false

            onClicked: root.updateObject();

            ToolTip {
                text: "Update the visualization of the data during simulation."
            }

            Timer {
                interval: 50
                repeat: true
                running: trackButton.checked

                onRunningChanged: root.updateObject();
                onTriggered: root.updateObject();
            }
        }

//        Button {
//            visible: dataObject.modified
//            text: "Undo"
//            onClicked: root.updateObject();

//            ToolTip {
//                description: "Undo changes in the data value"
//            }
//        }

//        Button {
//            Layout.columnSpan: 3
//            Layout.fillWidth: true
//            visible: dataObject.modified
//            text: "Update"
//            onClicked: root.updateData();

//            ToolTip {
//                description: "Update the data value"
//            }
//        }
    }

    onSofaDataChanged:
    {
        updateObject();

        /// Returns the widget's properties associated with this SofaData
        var component = SofaDataWidgetFactory.getWidgetForData(dataObject)
        component.createObject(datawidget, {"dataObject": dataObject, "Layout.fillWidth":true})
    }
}

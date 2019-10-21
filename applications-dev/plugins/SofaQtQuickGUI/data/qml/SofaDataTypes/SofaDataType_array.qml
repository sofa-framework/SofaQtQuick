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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4 as QQC1
import SofaBasics 1.0
import Sofa.Core.SofaData 1.0
import SofaApplication 1.0
import SofaColorScheme 1.0
import SofaDataContainerListModel 1.0

ColumnLayout {
    id: root
    spacing: 0

    property SofaData sofaData: null
    property var properties: sofaData.properties
    property int refreshCounter: 0

    Loader {
        id: loader
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: item.height
        sourceComponent: {
            console.log(sofaData.name)
            if(properties.static) {
                if((!properties.innerStatic && sofaData.value.length <= 7) ||
                   (properties.innerStatic && 1 === sofaData.value.length && properties.cols <= 7)) {
                    console.log ("staticSmallArrayView")
                    return staticSmallArrayView;
                } else if(properties.innerStatic && properties.cols <= 7) {
                    console.log ("staticInStaticTableView")
                    return staticInStaticTableView;
                } else {
                    console.log ("staticArrayView")
                    return staticArrayView;
                }
            }
            else {
                if(properties.innerStatic) {
                    if(properties.cols <= 7 || properties.cols === 12) {
                        // Case of Affine type
                        console.log("staticInDynamicTableView")
                        return staticInDynamicTableView;
                    } else {
                        console.log("dynamicArrayView")
                        return dynamicArrayView;
                    }
                }
            }

            console.log("dynamicArrayView")
            return dynamicArrayView;
        }

        Component {
            id: staticInStaticTableView
            QQC1.TableView {
                id: tableView

                rowDelegate: Rectangle {
                    color: styleData.selected ? "#82878c" : styleData.alternate ? SofaApplication.style.alternateBackgroundColor : SofaApplication.style.contentBackgroundColor
                }
                headerDelegate: GBRect {
                    color: "#757575"
                    border.width: 1
                    border.color: "#393939"
                    height: 20
                    width: textItem.implicitWidth
                    Text {
                        id: textItem
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: styleData.textAlignment
                        anchors.leftMargin: 12
                        text: { console.log("Values: " + styleData.value + " " + styleData.row + " " + styleData.column); return styleData.value }
                        elide: Text.ElideRight
                        renderType: Text.NativeRendering
                    }
                }
                Component {
                    id: columnComponent
                    QQC1.TableViewColumn {
                        movable: false
                        resizable: true
                        horizontalAlignment: Text.AlignHCenter
                        width: (tableView.width - 58) / (tableView.columnCount - 1)
                        Component.onCompleted: {
                            resizeToContents()
                        }
                    }
                }


                Component {
                    id: columnLines
                    QQC1.TableViewColumn {
                        movable: false
                        resizable: true
                        horizontalAlignment: Text.AlignHCenter
                        width: 58
                        Component.onCompleted: {
                            resizeToContents()
                        }
                    }
                }


                model: SofaDataContainerListModel {
                    id: listModel
                    sofaData: root.sofaData
                    Component.onCompleted: {
                        addColumn(columnLines.createObject(tableView, { "title": "", "role": 0}));
                        for (var i = 0 ; i < sofaData.properties.cols ; i++)
                            addColumn(columnComponent.createObject(tableView, { "id": ""+i, "title": headerData(i, Qt.Horizontal, ""), "role": i}));
                    }
                }

//                itemDelegate: TextInput {
//                    anchors.fill: parent
//                    text: listModel.data(listModel.index(styleData.row, 0), styleData.column)
//                    readOnly: true
//                }

                itemDelegate: TextField {
                    id: cell

                    Component {
                        id: bgComponent
                        Rectangle{ color: "transparent" }
                    }

                    anchors.fill: parent
                    anchors.leftMargin: -1
                    anchors.rightMargin: -1
                    clip: true
                    readOnly: sofaData.isReadOnly || styleData.column === 0
                    color: styleData.textColor
                    horizontalAlignment: TextEdit.AlignHCenter
                    verticalAlignment: TextEdit.AlignVCenter
                    validator: DoubleValidator {}
                    function getText() {
                        if (styleData.column === 0)
                            cell.text = styleData.row;
                        else if(-1 !== styleData.row && styleData.column !== 0) {
                            cell.text = Number(listModel.data(listModel.index(styleData.row, 0), styleData.column)).toPrecision(6);
                        }
                        else {
                            cell.text = "#REF!" // Hum... maybe a better "invalid cell" convention than ms excel....?
                        }
                        return cell.text
                    }
                    text: getText()
                    property int previousRow: -1
                    position: cornerPositions['Middle']

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        acceptedButtons: cell.readOnly ? Qt.NoButton : Qt.LeftButton
                        onClicked: {
                            parent.forceActiveFocus()
                        }
                    }
                    activeFocusOnTab: styleData.column !== 0 ? true : false
                    onActiveFocusChanged: {
                        if (!cell.readOnly) {
                            if (activeFocus) {
                                text = listModel.data(listModel.index(styleData.row, 0), styleData.column)
                                mouseArea.cursorShape = Qt.IBeamCursor
                                background.color = "#82878c"
                            } else {
                                mouseArea.cursorShape = Qt.ArrowCursor
                                background.color = "transparent"
                                text = getText()
                            }
                        }
                    }

                    onEditingFinished: {
                        if(-1 === styleData.row || sofaData.isReadOnly || 0 === styleData.column)
                            return;

                        if(previousRow !== styleData.row) {
                            previousRow = styleData.row;
                            return;
                        }

                        if(styleData.column !== 0) {
                            var oldValue = listModel.data(listModel.index(styleData.row, 0), styleData.column);
                            var value = text;
                            if(value !== oldValue) {
                                listModel.setData(listModel.index(styleData.row, 0), text, styleData.column)
                            }
                        }
                        text = getText()
                    }
                    Component.onCompleted: {
                        if (0 === styleData.column) cell.background = bgComponent.createObject(cell)
                    }
                }
            }
        }

        Component {
            id: staticSmallArrayView
            RowLayout {
                id: rowLayout
                Layout.preferredWidth: (parent.width - 1) / sofaData.value.length + 1
                spacing: -1
                enabled: !sofaData.readOnly

                property var fields: []
                property bool innerArray: false

                Component.onCompleted: populate();

                function populate() {
                    var values = sofaData.value;
                    if(1 === values.length && Array.isArray(values[0]))
                    {
                        values = sofaData.value[0];
                        innerArray = true;
                    }

                    fields = [];
                    for(var i = 0; i < values.length; ++i)
                    {
                        fields[i] = textFieldComponent.createObject(rowLayout, {index: i});
                        if (i === 0)
                            fields[i].position = fields[i].cornerPositions['Left']
                        else if (i === values.length -1)
                            fields[i].position = fields[i].cornerPositions['Right']

                    }

                    update();
                }

                function update() {
                    var values = sofaData.value;
                    if(innerArray)
                        values = sofaData.value[0];

                    for(var i = 0; i < values.length; ++i) {
                        fields[i].value = values[i];
                    }
                }

                Component {
                    id: textFieldComponent

                    SpinBox {
                        Layout.preferredWidth: (parent.width- 1) / (rowLayout.innerArray ? sofaData.value[0].length : sofaData.value.length) + 1
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignTop
                        enabled: !sofaData.readOnlyTableViewColumn
                        position: cornerPositions['Middle']
                        showIndicators: false


                        property int index

                        value: sofaData.value[0][index]
                        onValueChanged: {
                            if(rowLayout.innerArray) {
                                var val = sofaData.value
                                val[0][index] = value
                                sofaData.value = val;
                            }
                            else {
                                val = sofaData.value
                                val[index] = value
                                sofaData.value = val;
                            }
                            sofaData.modified = true;
                        }
                    }
                }

                Connections {
                    target: root.sofaData
                    onValueChanged: rowLayout.update();
                }
            }
        }

        Component {
            id: staticArrayView
            TextField {
                id: textField
                readOnly: root.sofaData.isReadOnly
                enabled: !root.sofaData.isReadOnly
                text: undefined !== root.sofaData.value ? root.sofaData.value.toString() : ""
                Binding {
                    target: root.sofaData
                    property: "value"
                    value: textField.text
                }
            }
        }

        Component {
            id: staticInDynamicTableView

            ColumnLayout {
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    spacing: -1
                    Text {
                        text: "Size "
                    }
                    SpinBox {
                        id: rowNumber
                        enabled: !root.sofaData.readOnly && showEditButton.checked
                        Layout.fillWidth: true
                        value: root.sofaData.value.length
                        onValueChanged : {
                            if(value === root.sofaData.value.length)
                                return;

                            var oldLength = root.sofaData.value.length;
                            root.sofaData.value.length = value;
                            for(var j = oldLength; j < root.sofaData.value.length; ++j) {
                                root.sofaData.value[j] = [];
                                for(var i = 0; i < properties.cols; ++i)
                                    root.sofaData.value[j][i] = 0;
                            }

                            root.sofaData.modified = true;

                            if(loader.item)
                                loader.item.populate();
                        }
                        position: cornerPositions["Left"]
                    }
                    Button {
                        id: showEditButton
                        text: "Edit"
                        checkable: true
                        position: cornerPositions["Right"]
                        enabled: rowNumber.value !== 0
                    }
                }

                Loader {
                    id: loader2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: showEditButton.checked
                    onVisibleChanged: {
                        if (visible) {
                            Layout.minimumHeight = 200
                            loader.Layout.minimumHeight = 224
                            root.Layout.minimumHeight = 224
                            parent.Layout.minimumHeight = 220
                        } else {
                            Layout.minimumHeight = 20
                            loader.Layout.minimumHeight = 20
                            root.Layout.minimumHeight = 20
                            parent.Layout.minimumHeight = 20
                        }
                    }

                    active: visible

                    sourceComponent: staticInStaticTableView
                }
                Rectangle {
                    visible: loader2.visible
                    color: "transparent"
                    implicitHeight: 1
                }

            }
        }

        Component {
            id: dynamicArrayView

            TextField {
                id: textField
                readOnly: sofaData & sofaData.readOnly
                enabled: !sofaData.readOnly

                onTextChanged: {
                    if(!sofaData.readOnly)
                        if(Array.isArray(sofaData.value))
                            sofaData.value = text.split(' ')
                        else
                            sofaData.value = text
                }

                Binding {
                    target: textField
                    property: "text"
                    value: Array.isArray(sofaData.value) ? sofaData.value.join(' ') : sofaData.value
                }
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

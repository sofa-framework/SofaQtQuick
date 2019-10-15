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

        sourceComponent: {
            if(properties.static) {
                if((!properties.innerStatic && sofaData.value.length <= 7) ||
                   (properties.innerStatic && 1 === sofaData.value.length && properties.cols <= 7))
                    return staticSmallArrayView;
                else if(properties.innerStatic && properties.cols <= 7)
                    return staticInStaticTableView;
                else
                    return staticArrayView;
            }
            else {
                if(properties.innerStatic) {
                    if(properties.cols <= 7 || properties.cols === 12) // Case of Affine type
                        return staticInDynamicTableView;
                    else
                        return dynamicArrayView;
                }
            }

            return dynamicArrayView;
        }

        Component {
            id: staticInStaticTableView
            QQC1.TableView {
                id: tableView

                Component {
                    id: columnComponent
                    QQC1.TableViewColumn {
                        movable: false
                        resizable: false
                        horizontalAlignment: Text.AlignHCenter
                        width: (tableView.width - 58) / (tableView.columnCount - 1)
                    }
                }

                Component {
                    id: columnLines
                    QQC1.TableViewColumn {
                        movable: false
                        resizable: false
                        horizontalAlignment: Text.AlignHCenter
                        width: 58
                    }
                }

                Component.onCompleted: {
                    addColumn(columnLines.createObject(tableView, {"title": "", "role": "printLinesNumber"}));
                    for(var i = 0; i < properties.cols; ++i)
                        addColumn(columnComponent.createObject(tableView, {"title": i.toString(), "role": "c" + i.toString()}));
                }

                Connections {
                    target: root.sofaData
                    onValueChanged: {
                        listModel.update();

                        sofaData.modified = false;
                    }
                }

                model: ListModel {
                    id: listModel

                    Component.onCompleted: populate();

                    property int previousCount: 0

                    function populate() {
                        var newCount = sofaData.value.length;
                        if(previousCount < newCount)
                            for(var j = previousCount; j < newCount; ++j) {
                                var values = {};
                                for(var i = previousCount; i < properties.cols; ++i)
                                    values["c" + i.toString()] = sofaData.value[j][i];

                                append(values);
                            }
                        else if(previousCount > newCount)
                            remove(newCount, previousCount - newCount);

                        previousCount = count;
                    }

                    function update() {
                        if(count !== sofaData.value.length)
                            populate();

                        for(var j = 0; j < count; ++j) {
                            var values = {};
                            for(var i = previousCount; i < properties.cols; ++i)
                                values["c" + i.toString()] = sofaData.value[j][i];

                            set(j, values);
                        }
                    }
                }

                function populate() {
                    listModel.populate();
                }

                function update() {
                    listModel.update();
                }

                itemDelegate: TextInput {
                    anchors.fill: parent
                    anchors.leftMargin: 6
                    anchors.rightMargin: 6
                    clip: true
                    readOnly: -1 === styleData.row || sofaData.readOnly || 0 === styleData.column
                    color: styleData.textColor
                    horizontalAlignment: TextEdit.AlignHCenter
                    text: {
                        if (styleData.column === 0)
                            return styleData.row;
                        else if(-1 !== styleData.row && styleData.column !== 0) {
                            return sofaData.value[styleData.row][styleData.column - 1];
                        }

                        return "";
                    }
                    property int previousRow: -1
                    onTextChanged: {
                        if(-1 === styleData.row || sofaData.readOnly || 0 === styleData.column)
                            return;

                        if(previousRow !== styleData.row) {
                            previousRow = styleData.row;
                            return;
                        }

                        if(styleData.column !== 0) {
                            var oldValue = sofaData.value[styleData.row][styleData.column - 1];

                            var value = text;
                            if(value !== oldValue) {
                                sofaData.value[styleData.row][styleData.column - 1] = value;
                                sofaData.modified = true;
                            }
                        }
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
                            console.log(sofaData.value[0])
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
                readOnly: sofaData.readOnly
                enabled: !sofaData.readOnly
                text: undefined !== sofaData.value ? sofaData.value.toString() : ""
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
                        enabled: !sofaData.readOnly && showEditButton.checked
                        Layout.fillWidth: true
                        value: sofaData.value.length
                        onValueChanged : {
                            if(value === sofaData.value.length)
                                return;

                            var oldLength = sofaData.value.length;
                            sofaData.value.length = value;
                            for(var j = oldLength; j < sofaData.value.length; ++j) {
                                sofaData.value[j] = [];
                                for(var i = 0; i < properties.cols; ++i)
                                    sofaData.value[j][i] = 0;
                            }

                            sofaData.modified = true;

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
                    }
                }

                Loader {
                    id: loader
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: showEditButton.checked
                    active: visible
                    sourceComponent: staticInStaticTableView
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

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

    property SofaData dataObject: null
    property var properties: dataObject.properties
    property int refreshCounter: 0

    Loader {
        id: loader
        Layout.fillWidth: true
        Layout.fillHeight: true

        sourceComponent: {
            if(properties.static) {
                if((!properties.innerStatic && dataObject.value.length <= 7) ||
                   (properties.innerStatic && 1 === dataObject.value.length && properties.cols <= 7))
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
                    target: root.dataObject
                    onValueChanged: {
                        listModel.update();

                        dataObject.modified = false;
                    }
                }

                model: ListModel {
                    id: listModel

                    Component.onCompleted: populate();

                    property int previousCount: 0

                    function populate() {
                        var newCount = dataObject.value.length;
                        if(previousCount < newCount)
                            for(var j = previousCount; j < newCount; ++j) {
                                var values = {};
                                for(var i = previousCount; i < properties.cols; ++i)
                                    values["c" + i.toString()] = dataObject.value[j][i];

                                append(values);
                            }
                        else if(previousCount > newCount)
                            remove(newCount, previousCount - newCount);

                        previousCount = count;
                    }

                    function update() {
                        if(count !== dataObject.value.length)
                            populate();

                        for(var j = 0; j < count; ++j) {
                            var values = {};
                            for(var i = previousCount; i < properties.cols; ++i)
                                values["c" + i.toString()] = dataObject.value[j][i];

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
                    readOnly: -1 === styleData.row || dataObject.readOnly || 0 === styleData.column
                    color: styleData.textColor
                    horizontalAlignment: TextEdit.AlignHCenter
                    text: {
                        if (styleData.column === 0)
                            return styleData.row;
                        else if(-1 !== styleData.row && styleData.column !== 0) {
                            return dataObject.value[styleData.row][styleData.column - 1];
                        }

                        return "";
                    }
                    property int previousRow: -1
                    onTextChanged: {
                        if(-1 === styleData.row || dataObject.readOnly || 0 === styleData.column)
                            return;

                        if(previousRow !== styleData.row) {
                            previousRow = styleData.row;
                            return;
                        }

                        if(styleData.column !== 0) {
                            var oldValue = dataObject.value[styleData.row][styleData.column - 1];

                            var value = text;
                            if(value !== oldValue) {
                                dataObject.value[styleData.row][styleData.column - 1] = value;
                                dataObject.modified = true;
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
                //width: parent.width
                spacing: -1
                enabled: !dataObject.readOnly

                property var fields: []
                property bool innerArray: false

                Component.onCompleted: populate();

                function populate() {
                    var values = dataObject.value;
                    if(1 === values.length && Array.isArray(values[0]))
                    {
                        values = dataObject.value[0];
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
                    var values = dataObject.value;
                    if(innerArray)
                        values = dataObject.value[0];

                    for(var i = 0; i < values.length; ++i) {
                        fields[i].value = values[i];
                    }
                }

                Component {
                    id: textFieldComponent

                    SpinBox {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignTop
//                        readOnly: dataObject & dataObject.readOnly
                        enabled: !dataObject.readOnlyTableViewColumn
                        position: cornerPositions['Middle']
                        showIndicators: false


                        property int index

                        value: dataObject.value[0][index]
                        onValueChanged: {
                            if(rowLayout.innerArray)
                                dataObject.value[0][index] = value;
                            else
                                dataObject.value[index] = value;
                            dataObject.modified = true;
                        }
                    }
                }

                Connections {
                    target: root.dataObject
                    onValueChanged: rowLayout.update();
                }
            }
        }

        Component {
            id: staticArrayView
            TextField {
                id: textField
                readOnly: dataObject.readOnly
                enabled: !dataObject.readOnly
                text: undefined !== dataObject.value ? dataObject.value.toString() : ""
                Binding {
                    target: root.dataObject
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
                        enabled: !dataObject.readOnly && showEditButton.checked
                        Layout.fillWidth: true
                        value: dataObject.value.length
                        onValueChanged : {
                            if(value === dataObject.value.length)
                                return;

                            var oldLength = dataObject.value.length;
                            dataObject.value.length = value;
                            for(var j = oldLength; j < dataObject.value.length; ++j) {
                                dataObject.value[j] = [];
                                for(var i = 0; i < properties.cols; ++i)
                                    dataObject.value[j][i] = 0;
                            }

                            dataObject.modified = true;

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
                readOnly: dataObject & dataObject.readOnly
                enabled: !dataObject.readOnly

                onTextChanged: {
                    if(!dataObject.readOnly)
                        if(Array.isArray(dataObject.value))
                            dataObject.value = text.split(' ')
                        else
                            dataObject.value = text
                }

                Binding {
                    target: textField
                    property: "text"
                    value: Array.isArray(dataObject.value) ? dataObject.value.join(' ') : dataObject.value
                }
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

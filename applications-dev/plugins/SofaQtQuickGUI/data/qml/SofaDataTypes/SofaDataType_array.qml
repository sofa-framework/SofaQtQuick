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
import QtQuick.Controls 2.4
import SofaBasics 1.0
import Sofa.Core.SofaData 1.0
import SofaApplication 1.0
import SofaColorScheme 1.0
import SofaDataContainerListModel 1.0

ColumnLayout {
    id: root
    spacing: 0

    property SofaData sofaData
    property var properties: sofaData.properties
    property int refreshCounter: 0

    Loader {
        id: loader
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: item.height
        sourceComponent: properties.static ? staticContainerView : dynamicContainerView

        Component {
            id: staticContainerView
            GridView {
                id: gridView
                implicitHeight: listModel.sofaData ? listModel.rowCount() / listModel.columnCount() * 20 : 20
                Layout.fillWidth: true
                Layout.fillHeight: true
                cellWidth: gridView.width / (listModel.columnCount())
                cellHeight: 20
                model: SofaDataContainerListModel {
                    /*
                      ########### TableModel: ############
                      rowCount(): total number of rows
                      columnCount(): total number of columns
                      data(index, role): the data corresponding to the given role at the given index
                      setData(index, value, role): sets the data for a role at index
                      corner: Single / TopLeft / TopRight / BottomLeft / BottomRight / Middle
                      display: the cell's value
                     */
                    id: listModel
                    sofaData: root.sofaData
                }

                delegate: SpinBox {
                    id: cellItem
                    implicitWidth: gridView.cellWidth + 1
                    implicitHeight: 21
                    readOnly: sofaData.isReadOnly
                    position: cornerPositions[corner]
                    showIndicators: listModel.columnCount() === 1
                    value: dataValue
                    onValueChanged: {
                        console.log("plop")
                        listModel.setData(_index, value, 0)
                    }
                }
            }
        }

        Component {
            id: theTableView
            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                QQC1.TableView {
                    id: tableView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
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
                            text: styleData.value
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
                        }
                    }


                    Component {
                        id: columnLines
                        QQC1.TableViewColumn {
                            movable: false
                            resizable: true
                            horizontalAlignment: Text.AlignHCenter
                            width: 58
                        }
                    }


                    model: SofaDataContainerListModel {
                        id: listModel
                        sofaData: root.sofaData
                        Component.onCompleted: {
                            tableView.addColumn(columnLines.createObject(tableView, { "title": "", "role": "c0"}));
                            for (var i = 0 ; i < sofaData.properties.cols ; i++)
                                tableView.addColumn(columnComponent.createObject(tableView, { "id": ""+i, "title": headerData(i, Qt.Horizontal, ""), "role": "c"+i}));
                        }
                        asGridViewModel: false
                    }


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
                                    cell.selectAll()
                                } else {
                                    mouseArea.cursorShape = Qt.ArrowCursor
                                    background.color = "transparent"
                                    text = getText()
                                }
                            }
                        }

                        onEditingFinished: {
                            cell.focus = false
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
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    enabled: sofaData? !sofaData.isReadOnly : false
                    spacing: -1
                    Repeater {
                        id: repeaterId
                        model: sofaData.properties.cols
                        delegate: SpinBox {
                            Layout.fillWidth: true
                            value: 0
                            position: cornerPositions[index == 0 ? 'Left' : 'Middle']

                        }
                    }
                    Button {
                        text: "Add row"
                        onClicked: {
                            var list = []
                            for (var i = 0 ; i < repeaterId.count ; ++i) {
                                list[i] = repeaterId.itemAt(i).value
                            }
                            listModel.insertRow(list)

                        }
                        position: cornerPositions['Right']
                    }
                }
            }
        }

        Component {
            id: dynamicContainerView

            ColumnLayout {
                spacing: 0
                id: tableViewParent
                RowLayout {
                    id: showTableLayoutId
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    spacing: -1
                    Text {
                        text: root.sofaData.value.length + (root.sofaData.value.length === 1 ? " Entry" : " Entries")
                        Layout.fillWidth: true
                    }

                    Button {
                        id: showEditButton
                        text: "Show"
                        checkable: true
                        position: cornerPositions["Right"]
                    }
                }

                Loader {
                    id: loader2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: showEditButton.checked
                    function getTableWindowHeight(nEntries) {
                        return ((nEntries === 0 ? 0 : nEntries > 10 ? 10 : nEntries - 1)) * 20 + 40
                    }

                    onVisibleChanged: {
                        if (visible) {
                            Layout.preferredHeight = getTableWindowHeight(sofaData.value.length)
                            loader.Layout.preferredHeight = getTableWindowHeight(sofaData.value.length) + showTableLayoutId.implicitHeight
                            root.Layout.preferredHeight = getTableWindowHeight(sofaData.value.length) + showTableLayoutId.implicitHeight
                            parent.Layout.preferredHeight = getTableWindowHeight(sofaData.value.length) + showTableLayoutId.implicitHeight
                        } else {
                            root.implicitHeight = 20
                            root.preferredHeight = 20
                            root.height = 20
                            root.minimumHeight = 20
                            root.maximumHeight = 20
                        }
                    }

                    active: visible

                    sourceComponent: theTableView
                }
            }
        }
    }
}

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
    property alias sofaData: listModel.sofaData
    Layout.minimumHeight: tableView.implicitHeight + addRowId.implicitHeight
    spacing: 0
    QQC1.TableView {
        id: tableView


        model: SofaDataContainerListModel {
            id: listModel
            Component.onCompleted: {
                tableView.addColumn(indicesColumn.createObject(tableView, { "title": "", "role": "c0"}));
                for (var i = 0 ; i < root.sofaData.properties.cols ; i++)
                    tableView.addColumn(valuesColumn.createObject(tableView, { "id": ""+i, "title": headerData(i, Qt.Horizontal, ""), "role": "c"+i}));
            }
            asGridViewModel: false
        }


        Layout.fillWidth: true
        implicitHeight: (rowCount <= 10 ? rowCount * 16 : 160) + 20

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
            id: valuesColumn
            QQC1.TableViewColumn {
                movable: false
                resizable: true
                horizontalAlignment: Text.AlignHCenter
                width: (tableView.width - 58) / (tableView.columnCount - 1)
            }
        }

        Component {
            id: indicesColumn
            QQC1.TableViewColumn {
                movable: false
                resizable: true
                horizontalAlignment: Text.AlignHCenter
                width: 58
            }
        }


        onRowCountChanged: {
            tableView.selection.clear()
            tableView.selection.select(rowCount - 1)
            tableView.__listView.positionViewAtEnd()
        }

        Component.onCompleted: tableView.selection.clear()

        itemDelegate: Loader {
            anchors.fill: parent
            id: itemDelegateLoader


            Component {
                id: lastRowIdxComponent
                Button {
                    property var styleData: parent.styleData
                    onClicked: {
                        listModel.removeLastRow()
                    }
                    Image {
                        width: 10
                        height: 10
                        source: "qrc:/icon/invalid.png"
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit
                    }
                }
            }

            Component {
                id: cellComponent
                TextField {
                    id: cell
                    property var styleData: parent.styleData

                    Rectangle {
                        id: idxBg
                        anchors.fill: parent
                        color: "transparent"
                    }

                    anchors.fill: parent
                    anchors.leftMargin: -1
                    anchors.rightMargin: -1
                    clip: true
                    readOnly: root.sofaData.properties.readOnly || styleData.column === 0
                    color: styleData.textColor
                    horizontalAlignment: TextEdit.AlignHCenter
                    verticalAlignment: TextEdit.AlignVCenter
                    validator: DoubleValidator {}
                    function getText() {
                        if (styleData.column === 0)
                            var txt = parseInt(Number(listModel.data(listModel.index(styleData.row, 0), styleData.column)).toPrecision(6)).toString();
                        else if(-1 !== styleData.row) {
                            txt = Number(listModel.data(listModel.index(styleData.row, 0), styleData.column)).toPrecision(6);
                        }
                        else {
                            txt = "#REF!" // Hum... maybe a better "invalid cell" convention than ms excel....?
                        }
                        return txt
                    }
                    text: getText()
                    property bool previousRow: false
                    position: cornerPositions['Middle']

                    activeFocusOnTab: styleData.column !== 0 ? true : false
                    onActiveFocusChanged: {
                        if (!cell.readOnly) {
                            if (activeFocus) {
                                background.color = "#82878c"
                                cell.selectAll()
                            } else {
                                background.color = "transparent"
                            }
                        }
                    }

                    onEditingFinished: {
                        cell.focus = false
                        if(-1 === styleData.row || sofaData.properties.readOnly || 0 === styleData.column)
                            return;

                        if(styleData.column !== 0) {
                            var oldValue = listModel.data(listModel.index(styleData.row, 0), styleData.column);
                            var value = text;
                            if(value !== oldValue) {
                                listModel.setData(listModel.index(styleData.row, 0), text, styleData.column)
                                sofaData.persistent = true;
                            }
                        }
                        var v = getText()
                        if (v === value)
                            text = v
                    }

                    background: styleData.column === 0 ? idxBg : background
                }
            }


            sourceComponent: {
                console.log("c: "+ styleData.column + " , r: " + styleData.row + "\t v:" + Number(listModel.data(listModel.index(styleData.row, 0), styleData.column)) );
                return styleData.column === 0 && styleData.row === tableView.rowCount -1 ?
                            lastRowIdxComponent : cellComponent
            }
            onLoaded: item.styleData = styleData
        }

    }
    RowLayout {
        id: addRowId
        Layout.fillWidth: true
        implicitHeight: 20
        enabled: root.sofaData? !root.sofaData.properties.readOnly : false
        spacing: -1
        Repeater {
            id: repeaterId
            model: root.sofaData.properties.cols
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

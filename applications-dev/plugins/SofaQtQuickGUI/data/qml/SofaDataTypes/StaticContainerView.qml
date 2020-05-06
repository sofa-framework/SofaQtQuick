import QtQuick 2.12
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls 2.4
import SofaBasics 1.0
import Sofa.Core.SofaData 1.0
import SofaApplication 1.0
import SofaColorScheme 1.0
import SofaDataContainerListModel 1.0

GridView {
    id: gridview
    property alias sofaData: listModel.sofaData

    implicitHeight: listModel.columnCount() ? listModel.rowCount() / listModel.columnCount() * 20 : 20
    Layout.fillWidth: true
    Layout.fillHeight: true
    cellWidth: width / listModel.columnCount()
    cellHeight: 20
    model: SofaDataContainerListModel {
        id: listModel
    }

    delegate: SpinBox {
        id: cellItem
        implicitWidth: gridview.cellWidth + 1
        implicitHeight: 21
        readOnly: listModel.sofaData.properties.readOnly
        position: cornerPositions[corner]
        showIndicators: listModel.columnCount() === 1
        value: dataValue
        onValueEditted: {
            listModel.setData(_index, value, 0)
        }
    }

}

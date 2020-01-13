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
    spacing: 0
    id: root
    Layout.fillWidth: true
    property SofaData sofaData

    implicitHeight: tableLoader.implicitHeight + showTableLayoutId.implicitHeight

    RowLayout {
        id: showTableLayoutId
        Layout.fillWidth: true
        implicitHeight: 20
        Layout.alignment: Qt.AlignTop
        spacing: -1
        Text {
            id: nEntriesLbl
            Layout.fillWidth: true
//            text: {
//                return root.sofaData.value.length + " Entries"
//            }
        }

        Button {
            id: showEditButton
            text: "Show"
            checkable: true
            position: cornerPositions["Right"]
        }
    }

    Loader {
        id: tableLoader
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: visible && item ? item.Layout.minimumHeight : 0
        visible: showEditButton.checked
        active: visible

        sourceComponent: DynamicTableView {
            sofaData: root.sofaData
        }
    }
}

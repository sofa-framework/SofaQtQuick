import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import Asset 1.0
import PythonAsset 1.0

Item {
    id: root
    property Asset selectedAsset

    Column {

        spacing: 10
        Repeater {
            model: selectedAsset.scriptContent
            GroupBox {
                width: root.parent.width
                title: modelData.name
                Rectangle {
                    color: 'transparent'
                    width: root.parent.width / 4 * 3
                    implicitHeight: docstringLbl.implicitHeight
                    Label {
                        color: "#eeeeec"
                        id: docstringLbl
                        anchors.fill: parent
                        text: modelData.docstring
                        wrapMode: Text.Wrap
                    }
                }
            }
        }
    }
}

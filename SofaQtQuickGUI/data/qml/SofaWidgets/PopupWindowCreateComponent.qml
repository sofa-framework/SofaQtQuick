import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0
import SofaViews 1.0
import SofaWidgets 1.0
import SofaFactory 1.0

Component {
    Window {
        id: searchBar
        width: 400
        height: 400
        modality: Qt.NonModal
        flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
        visible: true
        color: "lightgrey"
        property QtObject sofaComponent : null

        ComboBox {
            id: textField
            anchors.left: searchBar.left
            anchors.right: searchBar.right

            editable: true
            model: SofaFactory.components
            //placeholderText: "Search component ..."
            //onTextChanged:
            //onAccepted: searchBar.validateFilter()
            //onEditTextChanged: searchBar.updateFilter(currentText)
            onAccepted: {
                sofaScene.createAndAddComponentTo(sofaComponent, currentText) ;
            }
        }

//        ListView {
//            anchors.fill: parent
//            anchors.top  : textField.bottom

//            model : SofaFactory.components
//            onModelChanged: {
//                console.log("COM: "+SofaFactory.components)
//            }
//        }

        function updateFilter(s)
        {
            console.log("Update filter with "+s)
            SofaFactory.setFilter(s)
            console.log("COM: "+SofaFactory.components)
        }

        function validateFilter()
        {
            console.log("Update filter")
        }
    }
}

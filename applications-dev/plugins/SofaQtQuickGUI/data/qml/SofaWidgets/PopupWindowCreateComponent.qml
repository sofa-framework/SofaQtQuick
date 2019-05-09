import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0
import SofaViews 1.0
import SofaWidgets 1.0
import Sofa.Core.SofaFactory 1.0

Window {
    id: searchBar
    width: 400
    height: 100
    modality: Qt.NonModal
    flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
    visible: true
    color: "lightgrey"
    property QtObject sofaComponent : null


    TextField {
        id: inputField
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20

        onTextEdited:
        {
            SofaFactory.setFilter(text)
        }

    }



    ListView {
        currentIndex: 1
        keyNavigationEnabled: true
        anchors.top: inputField.bottom
        anchors.left: inputField.left
        anchors.right: inputField.right
        anchors.bottom: parent.bottom
        model: SofaFactory.components
        delegate: Row
        {
            spacing: 10
            Rectangle{
                color:  ListView.isCurrentItem ? "orange" : "white"
                //Text { text: modelData }
            }
        }

        MouseArea {
            id: itemMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.itemSelected(delegateItem.suggestion)
        }

    }




    //        Component
    //        {
    //            id: myComponent
    //            Rectangle
    //            {
    //                property var field: null
    //                anchors.top : field.anchors.bottom
    //                anchors.left : field.anchors.left
    //                anchors.right : field.anchors.right
    //                height: 100

    //                color : "red"
    //            }
    //        }

    //        Loader
    //        {
    //            sourceComponent: myComponent
    //            field: inputField
    //        }

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

import QtQuick 2.0
import Sofa.Core.SofaFactory 1.0


Rectangle
{
    anchors.top: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right

    height : 100
    width : parent.width

    color : "red"
    z: -100

    ListModel
    {
        id : myModel
        ListElement {
            name : "ttA"
            cost: 1.5
        }
        ListElement {
            name : "ttB"
            cost: 2.0
        }
    }

    ListView
    {
        anchors.top: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        z:1
        model : myModel
    }
}

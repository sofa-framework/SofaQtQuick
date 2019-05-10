import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0
import SofaViews 1.0
import SofaWidgets 1.0
import Sofa.Core.SofaFactory 1.0

Popup {
    property QtObject sofaNode : null

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    id: searchBar
    contentWidth: 400
    contentHeight: 300
    padding: 10

    MessageDialog
    {
        visible: false
        id: messageDialog
        title: "Error"
        text: "Unable to create an object."
        onAccepted: {
            console.log("Of course you could only agree.")
            visible=false
        }
    }

    TextField {
        id: inputField
        anchors.top: searchBar.top
        anchors.left: searchBar.left
        width: searchBar.contentWidth
        height: 20
        font.italic: !SofaFactory.contains(text)

        onTextEdited:
        {
            SofaFactory.setFilter(text)
        }
        onAccepted:
        {
            if( SofaFactory.contains(text) )
            {
                sofaNode.clearWarning()
                var p=sofaNode.createObject(text, {"name" : sofaNode.getNextObjectName(text) })
                searchBar.close()
                if(p!==null)
                {
                    SofaApplication.signalComponent(p.getPathName());
                }else
                {
                     messageDialog.text =
                             "Unable to create an object of type '"+text+"' in node "+sofaNode.getName() + "\n"
                            +"Reason: " + sofaNode.warning() + " " + sofaNode.output()
                    messageDialog.visible = true
                }
            }
            SofaFactory.setFilter("")
        }
    }

    ListView {
        id: container
        currentIndex: 1
        keyNavigationEnabled: true
        anchors.top: inputField.bottom
        anchors.left: inputField.left
        anchors.right: inputField.right
        height: searchBar.contentHeight-inputField.height-(2*searchBar.padding)
        model: SofaFactory.components

        delegate: Component {
            Item{
                width: parent.width
                height: 20
                Rectangle {
                    id: area
                    anchors.fill: parent
                    color: container.currentIndex == index ? "lightsteelblue" : "gray"
                }
                Text {
                    text: modelData
                }
                ToolTip {
                    text: modelData
                    description:  SofaFactory.getComponentHelp(modelData)
                    visible: itemMouseArea.containsMouse
                    timeout: 2000
                }

                MouseArea {
                    id: itemMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        container.currentIndex = index
                        inputField.forceActiveFocus()
                        inputField.text = modelData
                    }
                }
            }
        }
    }

    function updateFilter(s)
    {
        SofaFactory.setFilter(s)
    }

    function validateFilter()
    {
        console.log("Update filter")
    }
}

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
    margins: 0

    property alias componentName: inputField.text
    property string templateName: ""

    MessageDialog
    {
        visible: false
        id: messageDialog
        title: "Error"
        text: "Unable to create an object."
        icon: StandardIcon.Critical
        onAccepted: {
            visible=false
        }
    }

    MessageDialog
    {
        visible: false
        id: messageCreateWidget
        title: "Warning"
        icon: StandardIcon.Critical
        text: "There is no component with such name. Do you want to create a new PythonController ?"

        onAccepted: {
            visible=false
            SofaApplication.currentProject.createPythonPrefab()
            searchBar.close()
        }
    }


    TextField {
        id: inputField
        anchors.top: searchBar.top
        anchors.left: searchBar.left
        width: searchBar.contentWidth
        height: 20
        font.italic: !SofaFactory.contains(text)
        focus: true
        selectByMouse: true
        Keys.forwardTo: [container]
        Keys.onPressed:
        {
            if(event.key === Qt.Key_Return)
            {
                console.log("inputField Ret pressed")
                inputField.text = SofaFactory.components[container.currentIndex]
            }
            if(Qt.Key_Down === event.key)
            {
                container.incrementCurrentIndex()
                text = null
            }
            if(Qt.Key_Up === event.key)
            {
                container.decrementCurrentIndex()
                text = null
            }
        }

        onTextEdited:
        {
            SofaFactory.setFilter(text)
            container.currentIndex=-1
        }

        onAccepted:
        {
            if( SofaFactory.contains(text) )
            {
                sofaNode.clearWarning()
                if (templateName !== "")
                    var p=sofaNode.createObject(text, {"name" : sofaNode.getNextObjectName(text), "template" : templateName })
                else
                    p=sofaNode.createObject(text, {"name" : sofaNode.getNextObjectName(text) })
                searchBar.close()
                if(p!==null)
                {
                    SofaApplication.signalComponent(p.getPathName());
                }else{
                    messageDialog.text =
                            "Unable to create an object of type '"+text+"' in node "+sofaNode.getName() + "\n"
                           +"Reason: " + sofaNode.warning() + " " + sofaNode.output()

                    messageDialog.visible = true
                }
            }
            else{
                messageCreateWidget.visible = true
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

        Keys.onRightPressed: {
            currentItem.showTemplates()
        }
        delegate: ListViewDelegate {
            listView: container
            ToolTip {
                text: modelData
                description:  SofaFactory.getComponentHelp(modelData)
                visible: itemMouseArea.containsMouse
                timeout: 2000
            }

            function showTemplates()
            {
                if (SofaFactory.getComponentTemplates(modelData).length > 1) {
                    templatesMenu.popup(x + width, y - index * height)
                }
            }

            MouseArea {
                id: itemMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if (container.currentIndex === index) {
                        if (SofaFactory.getComponentTemplates(modelData).length > 1 && !templatesMenu.opened)
                            parent.showTemplates()
                        else {
                            inputField.text = modelData
                            inputField.accepted()
                            templatesMenu.close()
                        }
                    } else {
                        container.currentIndex = index
                        inputField.forceActiveFocus()
                        parent.showTemplates()
                    }
                }

                Menu {
                    id: templatesMenu
                    onClosed: {
                        inputField.forceActiveFocus()
                    }
                    onOpened: {
                        contentData[0].forceActiveFocus()
                    }

                    Repeater {
                        model: SofaFactory.getComponentTemplates(modelData)
                        MenuItem {
                            text: modelData
                            Keys.onLeftPressed: {
                                templatesMenu.close()
                            }
                            Keys.onReturnPressed: {
                                accept()
                            }
                            Keys.onEnterPressed: {
                                accept()
                            }
                            function accept() {
                                templatesMenu.close()
                                searchBar.templateName = text
                                inputField.text = SofaFactory.components[container.currentIndex]
                                inputField.accepted()
                            }

                            onClicked: {
                                accept()
                            }
                        }
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

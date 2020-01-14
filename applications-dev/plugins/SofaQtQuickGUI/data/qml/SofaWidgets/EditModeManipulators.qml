import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import SofaApplication 1.0
import SofaInteractors 1.0
import EditView 1.0
import SofaColorScheme 1.0
import SofaBasics 1.0
import SofaManipulators 1.0

Column {
    id: manipulatorControls
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.leftMargin: 20
    anchors.topMargin: 20

    function getManipulator(manipulatorString) {
        var manipulator = Qt.createComponent("qrc:/SofaManipulators/" + manipulatorString + ".qml")
        if (manipulator.status === Component.Ready)
        {
            console.log("Created Manipulator with name " + manipulatorString)
            var m = manipulator.createObject()
            m.isEditMode = true
            return m
        }
        console.log("Cant create Manipulator with name " + manipulatorString)
        return null
    }


    Rectangle {
        id: translateRect
        property string manipulatorName: "Translate_Manipulator"

        property bool selected: SofaApplication.selectedManipulator && SofaApplication.selectedManipulator.name === manipulatorName

        function setManipulator() {
            var m = manipulatorControls.getManipulator(manipulatorName)
            if (m !== null)
                SofaApplication.selectedManipulator = m
        }

        implicitHeight: 30
        implicitWidth: 30
        color: "transparent"
        Rectangle {
            anchors.fill: parent
            color: translateRect.selected ? "#8888ff" : "white"
            opacity: translateRect.selected ? 0.7 : translateMarea.containsMouse ? 0.2 : 0.1
        }

        Image {
            anchors.centerIn: parent
            source: "qrc:/icon/ICON_TRANLSATION_MODIFIER.png"
            scale: 1.2
            opacity: 0.9
        }

        MouseArea {
            id: translateMarea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton

            onClicked: {
                translateRect.setManipulator()
            }
            Shortcut {
                context: Qt.ApplicationShortcut
                sequence: "Shift+Space, G";
                onActivated: {
                    translateRect.setManipulator()
                }
            }
        }
        ToolTip {
            visible: translateMarea.containsMouse
            text: "Move"
            description: "Translates the selected item\n Shortcut: Shift+Space, G"
        }
    }

    ManipulatorMenu {
        id: rotateMenu

        property string manipulatorName: "Rotate_Manipulator"

        selected: SofaApplication.selectedManipulator && SofaApplication.selectedManipulator.name === manipulatorName

        function setManipulator() {
            var m = manipulatorControls.getManipulator(manipulatorName)
            if (m !== null)
                SofaApplication.selectedManipulator = m
        }

        image: "qrc:/icon/ICON_ROTATION_MODIFIER.png"
        menuItemModel: ListModel {
            ListElement {
                title: "Local"
                option: true
            }
            ListElement {
                title: "Global"
                option: false
            }
        }
        onOptionSelected: {
            rotateMenu.setManipulator()
            SofaApplication.selectedManipulator.local = option
        }

        Shortcut {
            context: Qt.ApplicationShortcut
            sequence: "Shift+Space, R";
            onActivated: {
                rotateMenu.setManipulator()
                SofaApplication.selectedManipulator.local = true
            }
        }

        ToolTip {
            visible: rotateMenu.containsMouse
            text: "Rotate"
            description: "Rotates the selected item (default: local reference frame)\n Shortcut: Shift+Space, R"
        }

    }
}

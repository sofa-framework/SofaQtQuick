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
        
    ManipulatorMenu {
        id: translateMenu
        property string manipulatorName: "Translate_Manipulator"

        selected: SofaApplication.selectedManipulator && SofaApplication.selectedManipulator.name === manipulatorName
        onOptionChanged: {
            translateMenu.setManipulator()
        }


        function setManipulator() {
            var m = SofaApplication.createManipulator(manipulatorName)
            if (m !== null)
                SofaApplication.selectedManipulator = m
        }

        model: null
        image: "qrc:/icon/ICON_TRANLSATION_MODIFIER.png"
        Shortcut {
            context: Qt.ApplicationShortcut
            sequence: "Shift+Space, G";
            onActivated: {
                translateMenu.setManipulator()
            }
        }
        ToolTip {
            visible: translateMenu.containsMouse
            text: "Move"
            description: "Translates the selected item\n Shortcut: Shift+Space, G"
        }
    }

    ManipulatorMenu {
        id: rotateMenu

        property string manipulatorName: "Rotate_Manipulator"

        selected: SofaApplication.selectedManipulator && SofaApplication.selectedManipulator.name === manipulatorName

        function setManipulator() {
            var m = SofaApplication.createManipulator(manipulatorName)
            if (m !== null)
                SofaApplication.selectedManipulator = m
        }

        image: "qrc:/icon/ICON_ROTATION_MODIFIER.png"
        model: ListModel {
            ListElement {
                title: "Local"
                option: true
            }
            ListElement {
                title: "Global"
                option: false
            }
        }
        onOptionChanged: {
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

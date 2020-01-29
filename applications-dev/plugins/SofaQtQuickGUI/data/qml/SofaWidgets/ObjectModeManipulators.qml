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

    function setManipulator(manipulatorName) {
        var m = SofaApplication.getManipulator(manipulatorName)
        if (!SofaApplication.getManipulators()) return m
        for (var i = 0 ; i < SofaApplication.getManipulators().length ; ++i) {
            if (!SofaApplication.getManipulators()[i].persistent &&
                    (m && m.name !== SofaApplication.getManipulators()[i].name)) {
                SofaApplication.getManipulators()[i].enabled = false
            }
        }
        m.enabled = true
        return m
    }

    ManipulatorMenu {
        id: translateMenu
        property string manipulatorName: "Translate_Manipulator"
        manipulator: SofaApplication.getManipulator(manipulatorName)

        onOptionChanged: {
            manipulator = setManipulator(manipulatorName)
        }

        model: null
        image: "qrc:/icon/ICON_TRANLSATION_MODIFIER.png"
        Shortcut {
            context: Qt.ApplicationShortcut
            sequence: "Shift+Space, G";
            onActivated: {
                manipulator = setManipulator(translateMenu.manipulatorName)
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
        manipulator: SofaApplication.getManipulator(manipulatorName)

        image: manipulator.local ? "qrc:/icon/ICON_ROTATION_MODIFIER.png" : "qrc:/icon/ICON_ROTATION_WORLD_MODIFIER.png"
        model: ListModel {
            ListElement {
                title: "Local"
                option: true
                iconSource: "qrc:/icon/ICON_ROTATION_MODIFIER.png"
            }
            ListElement {
                title: "Global"
                option: false
                iconSource: "qrc:/icon/ICON_ROTATION_WORLD_MODIFIER.png"
            }
        }

        onOptionChanged: {
            manipulator = setManipulator(manipulatorName)
            manipulator.local = option
        }

        Shortcut {
            context: Qt.ApplicationShortcut
            sequence: "Shift+Space, R";
            onActivated: {
                manipulator = manipulatorControls.setManipulator(manipulatorName)
                manipulator.local = true
            }
        }

        ToolTip {
            visible: rotateMenu.containsMouse
            text: "Rotate"
            description: "Rotates the selected item (default: local reference frame)\n Shortcut: Shift+Space, R"
        }
    }
}

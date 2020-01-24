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
        var m = SofaApplication.createManipulator(manipulatorName)
        if (m !== null)
        {
            for (var manip in SofaApplication.manipulators)
                if (!manip.persistent)
                    manip.enabled = false
            m.enabled = true
            return m
        }
        return null
    }

    Component.onCompleted: {
        var m = setManipulator("Viewpoint_Manipulator")
        m.persistent = true
        m.enabled = true
    }


    ManipulatorMenu {
        id: translateMenu
        property string manipulatorName: "Translate_Manipulator"

        selected: {
            for (var m in SofaApplication.manipulators)
                if (m.name === manipulatorName)
                    return m.enabled
            return false
        }

        onOptionChanged: {
            setManipulator(manipulatorName)
            selected = true
        }


        model: null
        image: "qrc:/icon/ICON_TRANLSATION_MODIFIER.png"
        Shortcut {
            context: Qt.ApplicationShortcut
            sequence: "Shift+Space, G";
            onActivated: {
                setManipulator(translateMenu.manipulatorName)
                selected = true
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

        selected: {
            for (var m in SofaApplication.manipulators)
                if (m.name === manipulatorName)
                    return m.enabled
            return false
        }

        onOptionChanged: {
            var m = setManipulator(manipulatorName)
            m.local = option
            selected = true
        }

        Shortcut {
            context: Qt.ApplicationShortcut
            sequence: "Shift+Space, R";
            onActivated: {
                var m = setManipulator()
                m.local = true
                selected = true
            }
        }

        ToolTip {
            visible: rotateMenu.containsMouse
            text: "Rotate"
            description: "Rotates the selected item (default: local reference frame)\n Shortcut: Shift+Space, R"
        }
    }
}

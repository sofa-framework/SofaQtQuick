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
    property var sofaViewer
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.leftMargin: 20
    anchors.topMargin: 20


    function setManipulator(manipulatorName) {
        var m = sofaViewer.getManipulator(manipulatorName)
        if (!sofaViewer.manipulators) return m
        for (var i = 0 ; i < sofaViewer.manipulators.length ; ++i) {
            if (!sofaViewer.manipulators[i].persistent &&
                    (m && m.name !== sofaViewer.manipulators[i].name)) {
                sofaViewer.manipulators[i].enabled = false
                print("disabling " + sofaViewer.manipulators[i].name)
            }
        }
        m.enabled = true
        return m
    }

    ManipulatorMenu {
        id: translateMenu
        property string manipulatorName: "Translate_Manipulator"
        manipulator: sofaViewer.getManipulator(manipulatorName)

        onOptionChanged: {
            manipulator = setManipulator(manipulatorName)
        }

        model: null
        image: "qrc:/icon/ICON_TRANLSATION_MODIFIER.png"
        Shortcut {
            context: Qt.ApplicationShortcut
            sequence: "Shift+Space, G";
            onActivated: {
                setManipulator(translateMenu.manipulatorName)
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
        manipulator: sofaViewer.getManipulator(manipulatorName)

        image: "qrc:/icon/ICON_ROTATION_MODIFIER.png"
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
            print(option)
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

    ManipulatorMenu {
        id: snapMenu
        property string manipulatorName: "Snapping_Manipulator"
        manipulator: sofaViewer.getManipulator(manipulatorName)

        onOptionChanged: {
            manipulator = setManipulator(manipulatorName)
        }

        model: null
        image: "qrc:/icon/ICON_SNAP_MODE.png"
        Shortcut {
            context: Qt.ApplicationShortcut
            sequence: "Shift+Space, S";
            onActivated: {
                setManipulator(translateMenu.manipulatorName)
            }
        }
        ToolTip {
            visible: translateMenu.containsMouse
            text: "Snap"
            description: "Provided that a MechanicalObject is selected, snaps new DOFs on visual geometries\n Shortcut: Shift+Space, S"
        }
    }
}

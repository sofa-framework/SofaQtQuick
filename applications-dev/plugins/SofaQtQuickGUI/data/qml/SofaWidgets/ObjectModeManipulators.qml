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
    
    property var selectedComponent: SofaApplication.selectedComponent
    onSelectedComponentChanged: {
        if (SofaApplication.selectedComponent === null)
            SofaApplication.selectedManipulator = getManipulator(camViewRect.manipulatorName)

    }
    
    function getManipulator(manipulatorString) {
        var manipulator = Qt.createComponent("qrc:/SofaManipulators/" + manipulatorString + ".qml")
        if (manipulator.status === Component.Ready)
        {
            console.log("Created Manipulator with name " + manipulatorString)
            var m = manipulator.createObject()
            console.log("Created Manipulator with name " + m.name)
            return m
        }
        console.log("Cant create Manipulator with name " + manipulatorString)
        return null
    }

    
    Rectangle {
        id: camViewRect
        property string manipulatorName: "Manipulator3D_NOMANIP"
        property bool selected: !SofaApplication.selectedManipulator || SofaApplication.selectedManipulator.name === manipulatorName
        
        implicitHeight: 30
        implicitWidth: 30
        color: "transparent"
        Rectangle {
            anchors.fill: parent
            color: camViewRect.selected ? "#8888ff" : "white"
            opacity: camViewRect.selected ? 0.7 : camViewMarea.containsMouse ? 0.2 : 0.1
        }
        
        Image {
            anchors.centerIn: parent
            source: "qrc:/icon/ICON_VIEWPOINT_MODIFIER.png"
            scale: 1.2
            opacity: 0.9
        }
        
        MouseArea {
            id: camViewMarea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            
            onClicked: {
                SofaApplication.selectedManipulator = manipulatorControls.getManipulator(camViewRect.manipulatorName)
            }
            Shortcut {
                context: Qt.ApplicationShortcut
                sequence: "Shift+Space, Space";
                onActivated: {
                    SofaApplication.selectedManipulator = manipulatorControls.getManipulator(camViewRect.manipulatorName)
                }
            }
        }
        ToolTip {
            visible: camViewMarea.containsMouse
            text: "ViewPoint"
            description: "Can select components, no manipulator\n Shortcut: Shift+Space, Space"
        }
    }
    
    
    Rectangle {
        id: translateRect
        property string manipulatorName: "Manipulator3D_InPlaneTranslation"
        
        property bool selected: SofaApplication.selectedManipulator && SofaApplication.selectedManipulator.name === manipulatorName
        
        function setManipulator() {
            if (!SofaApplication.selectedComponent) {
                SofaApplication.selectedManipulator = getManipulator(camViewRect.manipulatorName)
                return;
            }
            var manipulator = manipulatorControls.getManipulator(manipulatorName)
            manipulator.visible = true
            manipulator.sofaObject = SofaApplication.selectedComponent
            SofaApplication.selectedManipulator = manipulator
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
    
    
    Rectangle {
        id: rotateRect
        property string manipulatorName: "Manipulator3D_Rotation"
        
        property bool selected: sofaScene.selectedManipulator && sofaScene.selectedManipulator.name === manipulatorName
        
        function setManipulator() {
            if (!SofaApplication.selectedComponent) {
                SofaApplication.selectedManipulator = getManipulator(camViewRect.manipulatorName)
                return;
            }
            var manipulator = getManipulator(manipulatorName)
            manipulator.visible = true
            manipulator.sofaObject = SofaApplication.selectedComponent
            sofaScene.selectedManipulator = manipulator
        }
        
        implicitHeight: 30
        implicitWidth: 30
        color: "transparent"
        Rectangle {
            anchors.fill: parent
            color: rotateRect.selected ? "#8888ff" : "white"
            opacity: rotateRect.selected ? 0.7 : rotateMarea.containsMouse ? 0.2 : 0.1
        }
        
        Image {
            anchors.centerIn: parent
            source: "qrc:/icon/ICON_ROTATION_MODIFIER.png"
            scale: 1.2
            opacity: 0.9
        }
        
        MouseArea {
            id: rotateMarea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            
            onClicked: {
                rotateRect.setManipulator()
            }
            Shortcut {
                context: Qt.ApplicationShortcut
                sequence: "Shift+Space, R";
                onActivated: {
                    rotateRect.setManipulator()
                }
            }
        }
        ToolTip {
            visible: rotateMarea.containsMouse
            text: "Rotation"
            description: "Rotates the selected item\n Shortcut: Shift+Space, R"
        }
    }
    
    
    //        Rectangle {
    //            id: scaleRect
    //            Component.onCompleted: {
    ////                addManipulator("Manipulator3D_Scale")
    //            }
    
    //            property bool selected: false
    //            onSelectedChanged: {
    //            }
    
    //            implicitHeight: 30
    //            implicitWidth: 30
    //            color: "transparent"
    //            Rectangle {
    //                anchors.fill: parent
    //                color: scaleRect.selected ? "#8888ff" : "white"
    //                opacity: scaleRect.selected ? 0.7 : scaleMarea.containsMouse ? 0.2 : 0.1
    //            }
    
    //            Image {
    //                anchors.centerIn: parent
    //                source: "qrc:/icon/ICON_SCALE_MODIFIER.png"
    //                scale: 1.2
    //                opacity: 0.9
    //            }
    
    //            MouseArea {
    //                id: scaleMarea
    //                anchors.fill: parent
    //                hoverEnabled: true
    //                acceptedButtons: Qt.LeftButton
    //                function trigger() {
    //                    translateRect.selected = false
    //                    rotateRect.selected = false
    //                    camViewRect.selected = false
    //                    scaleRect.selected = true
    //                }
    
    //                onClicked: {
    //                    scaleMarea.trigger()
    //                }
    //                Shortcut {
    //                    context: Qt.ApplicationShortcut
    //                    sequence: "Shift+Space, S";
    //                    onActivated: {
    //                        scaleMarea.trigger()
    //                    }
    //                }
    //            }
    //            ToolTip {
    //                visible: scaleMarea.containsMouse
    //                text: "Scale"
    //                description: "Scales the selected item\n Shortcut: Shift+Space, S"
    //            }
    //        }
}

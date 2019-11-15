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
    
    visible: SofaApplication.getInteractorName(SofaApplication.interactorComponent) !== "CameraMode"
    property var selectedComponent: SofaApplication.selectedComponent
    onSelectedComponentChanged: {
        console.log("NYANYANYA")
        if (SofaApplication.selectedComponent === null)
            SofaApplication.sofaScene.selectedManipulator = null
    }

    function addManipulator(manipulatorString) {
        var manipulator = SofaApplication.sofaScene.getManipulatorByName(manipulatorString)
        if (!manipulator) {
            manipulator = Qt.createComponent("qrc:/SofaManipulators/" + manipulatorString + ".qml").createObject()
            SofaApplication.sofaScene.addManipulator(manipulator)
        }
        return manipulator
    }
}

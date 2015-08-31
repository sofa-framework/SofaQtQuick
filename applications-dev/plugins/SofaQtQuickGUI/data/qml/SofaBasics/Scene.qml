import QtQuick 2.0
import QtQuick.Controls 1.0
import Qt.labs.settings 1.0
import ParticleInteractor 1.0
import Scene 1.0
import "qrc:/SofaCommon/SofaSettingsScript.js" as SofaSettingsScript
import "qrc:/SofaCommon/SofaToolsScript.js" as SofaToolsScript

Scene {
    id: root

    asynchronous: true
    header: ""
    source: ""
    sourceQML: ""
    property string statusMessage: ""

    onStatusChanged: {
        var path = source.toString().replace("///", "/").replace("file:", "");
        switch(status) {
        case Scene.Loading:
            statusMessage = 'Loading "' + path + '" please wait';
            break;
        case Scene.Error:
            statusMessage = 'Scene "' + path + '" issued an error during loading';
            break;
        case Scene.Ready:
            statusMessage = 'Scene "' + path + '" loaded successfully';
            SofaSettingsScript.Recent.add(path);
            break;
        }
    }

    // convenience
    readonly property bool ready: status === Scene.Ready

    // allow us to interact with the python script controller
    property var pythonInteractor: PythonInteractor {scene: root}

    // allow us to interact with the scene particles
    property var particleInteractor: ParticleInteractor {
        stiffness: 100

        onInteractingChanged: SofaToolsScript.Tools.overrideCursorShape = interacting ? Qt.BlankCursor : 0
    }

    function keyPressed(event) {
        if(event.modifiers & Qt.ShiftModifier)
            onKeyPressed(event.key);
    }

    function keyReleased(event) {
        //if(event.modifiers & Qt.ShiftModifier)
            onKeyReleased(event.key);
    }

	property var resetAction: Action {
        text: "&Reset"
        shortcut: "Ctrl+Alt+R"
        onTriggered: root.reset();
        tooltip: "Reset the simulation"
    }

    // GET AND SET SOFA DATA VALUE

    function dataValue(dataName) {
        if(arguments.length == 1) {
            return onDataValue(dataName);
        }

        console.debug("ERROR: Scene - using dataValue with an invalid number of arguments:", arguments.length);
    }

    function setDataValue(dataName) {
        if(arguments.length > 1){
            var packedArguments = [];
            for(var i = 1; i < arguments.length; i++)
                packedArguments.push(arguments[i]);

            return onSetDataValue(dataName, packedArguments);
        }

        console.debug("ERROR: Scene - using setDataValue with an invalid number of arguments:", arguments.length);
    }

    ///// MANIPULATOR

    function addManipulator(manipulator) {
        var manipulators = [];
        for(var i = 0; i < root.manipulators.length; ++i)
            manipulators.push(root.manipulators[i]);

        manipulators.push(manipulator);
        root.manipulators = manipulators;

        // if the added manipulator is a compound also add its children manipulators
        if(manipulator.manipulators && 0 !== manipulator.manipulators.length)
            for(var i = 0; i < manipulator.manipulators.length; ++i)
                addManipulator(manipulator.manipulators[i]);
    }

    function removeManipulator(manipulator) {
        var manipulators = [];
        for(var i = 0; i < root.manipulators; ++i)
            if(manipulator !== root.manipulators[i])
                manipulators.push(root.manipulators[i]);

        root.manipulators = manipulators;
    }

    function clearManipulators() {
        root.manipulators = [];
    }

    ///// INTERFACE

    readonly property Loader interfaceLoader: Loader {
        id: interfaceLoader
        asynchronous: true
        source: root.sourceQML
    }
}

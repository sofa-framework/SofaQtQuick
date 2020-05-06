/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import SofaApplication 1.0
import SofaParticleInteractor 1.0
import SofaBaseScene 1.0

SofaBaseScene
{
    id: root

    asynchronous: false
    header: ""
    source: ""
    sourceQML: ""
    property string statusMessage: ""

    onStatusChanged: {
        switch(status) {
        case SofaBaseScene.Null:
            statusMessage = 'SofaScene released';
            break;
        case SofaBaseScene.Loading:
            statusMessage = 'SofaScene loading "' + root.path + '" please wait';
            break;
        case SofaBaseScene.Unloading:
            SofaApplication.sofaMessageList.clear()
            statusMessage = 'SofaScene releasing "' + root.path + '" please wait';
            break;
        case SofaBaseScene.Error:
            statusMessage = 'SofaScene "' + root.path + '" issued an error during loading';
            SofaApplication.selectedComponent = null
            break;
        case SofaBaseScene.Ready:
            var fatals = SofaApplication.sofaMessageList.getFatalCount()
            var errors = SofaApplication.sofaMessageList.getErrorCount()
            var warnings = SofaApplication.sofaMessageList.getWarningCount()
            var deprecated = SofaApplication.sofaMessageList.getDeprecatedCount()
            if (fatals) {
                statusMessage = 'SofaScene "' + root.path + '" issued a fatal error during loading';
                SofaApplication.selectedComponent = null
                break;
            }
            else if (errors)
                statusMessage = 'SofaScene "' + root.path + '" loaded with ' + errors + " errors and " + warnings + " warnings";
            else if (warnings)
                statusMessage = 'SofaScene "' + root.path + '" loaded successfully with ' + warnings + " warnings";
            else if (deprecated)
                statusMessage = 'SofaScene "' + root.path + '" loaded successfully. Consider upgrading your scene: ' + deprecated + " deprecated messages";
            else
                statusMessage = 'SofaScene "' + root.path + '" loaded successfully';

            SofaApplication.sceneSettings.addRecent(root.path);
            SofaApplication.selectedComponent = SofaApplication.sofaScene.root();
            break;
        }
    }

    // convenience
    readonly property bool ready: status === SofaScene.Ready
    readonly property bool loading: status === SofaScene.Loading

    // allow us to interact with the python script controller
//    property var sofaPythonInteractor: SofaPythonInteractor {sofaScene: root}

    // allow us to interact with the sofa scene particles
    // by default there is no particle-interactor
    property var sofaParticleInteractor: null

    function mousePressed(mouseState, emitter)
    {
        onMousePressed(mouseState.button, mouseState.x, mouseState.y);
    }

    function mouseReleased(mouseState, emitter)
    {
        onMouseReleased(mouseState.button, mouseState.x, mouseState.y);
    }


    function mouseMove(mouseState, emitter)
    {
        onMouseMove(mouseState.x, mouseState.y);
    }

    function keyPressed(event) {
        console.log("KEY PRESSED");
        onKeyPressed(event.key);
    }

    function keyReleased(event) {
        onKeyReleased(event.key);
    }

    property var resetAction: Action {
        text: "&Reset"
        shortcut: "Ctrl+Alt+R"
        onTriggered: root.reset();
//        ToolTip {
//            text: "Reset the scene"
//        }
    }

    // SOFACOMPONENT

    /// \note you can use data with a SofaComponent and fill the name of the data or directly put
    /// the data path in the data field and not use the name parameter
    function dataValue(data, name) {
        if(!Qt.isQtObject(data))
            return onDataValueByPath(data);
        return onDataValueByComponent(data, name);
    }

    /// \note you can use data with a SofaComponent and fill the name of the data or directly put
    /// the data path in the data field and not use the name parameter (just ignore it), the other
    /// parameters will be taken as the values to set
    function setDataValue(data, name) {
        var argumentBegin = Qt.isQtObject(data) ? 2 : 1;
        if(arguments.length > argumentBegin){
            var packedArguments = [];
            for(var i = argumentBegin; i < arguments.length; i++)
                packedArguments.push(arguments[i]);

            if(1 === argumentBegin)
                return onSetDataValueByPath(data, packedArguments);
            else
                return onSetDataValueByComponent(data, name, packedArguments);
        }

        console.debug("ERROR: SofaScene - using setDataValue with an invalid number of arguments:",
                      arguments.length);
    }

    ///// MANIPULATORS

    // add manipulator and its children
    function addManipulator(manipulator) {
        var manipulators = [];
        for(var i = 0; i < root.manipulators.length; ++i)
            manipulators.push(root.manipulators[i]);
        manipulators.push(manipulator);
        root.manipulators = manipulators;
        // if the added manipulator is a compound also add its children manipulators
        if(manipulator.manipulators && 0 !== manipulator.manipulators.length)
            for(var ii = 0; ii < manipulator.manipulators.length; ++ii)
                addManipulator(manipulator.manipulators[ii]);

        return manipulator;
    }

    property QtObject _manipulatorPrivate: QtObject {
        // function for internal use
        function removeManipulatorList(manipulatorList) {
            var manipulators = [];
            for(var i = 0; i < root.manipulators.length; ++i)
                if(-1 !== manipulatorList.indexOf(root.manipulators[i]))
                    manipulators.push(root.manipulators[i]);
            root.manipulators = manipulators;
        }
    }

    // remove manipulator and its children
    function removeManipulator(manipulator) {
        var manipulatorsToRemove = [];
        var index = root.manipulators.indexOf(manipulator);
        if(-1 !== index) {
            manipulatorsToRemove.push(root.manipulators[i]);
            // if the removed manipulator is a compound also remove its children manipulators
            if(manipulator.manipulators && 0 !== manipulator.manipulators.length)
                for(var j = 0; j < manipulator.manipulators.length; ++j)
                    manipulatorsToRemove.push(manipulator.manipulators[j]);
        }
        _manipulatorPrivate.removeManipulatorList(manipulatorsToRemove);
    }

    /** remove all manipulators called \a name and their children
     *  \return the found manipulators with name
     */
    function removeManipulatorByName(name) {
        var manipulatorsToRemove = [];
        var childrenManipulatorsToRemove = [];
        for(var i = 0; i < root.manipulators.length; ++i) {
            if(name === root.manipulators[i].name) {
                var manip = root.manipulators[i];
                manipulatorsToRemove.push(manip);
                // if the removed manipulator is a compound also remove its children manipulators
                if(manip.manipulators && 0 !== manip.manipulators.length)
                    for(var j = 0; j < manip.manipulators.length; ++j)
                        childrenManipulatorsToRemove.push(manip.manipulators[j]);
            }
        }
        _manipulatorPrivate.removeManipulatorList(manipulatorsToRemove);
        _manipulatorPrivate.removeManipulatorList(childrenManipulatorsToRemove);
        return manipulatorsToRemove;
    }

    function getManipulatorByName(name) {
        for(var i = 0; i < root.manipulators.length; ++i) {
            if(name === root.manipulators[i].name)
                return root.manipulators[i];
        }
        return null;
    }

    function clearManipulators() {
        root.manipulators = [];
    }


    ///// INTERFACE
    readonly property Loader interfaceLoader: Loader {
        id: interfaceLoader
        asynchronous: false

        // delay the load of the interface to one frame to let the cache be trimmed in order to load
        // the scene interface from scratch without reusing the previous loaded one in case of a
        // scene reloading
        Timer {
            id: loadInterfaceTimer
            interval: 1
            repeat: false
            running: root.ready
            onTriggered: {
                interfaceLoader.source = root.sourceQML;
            }
        }

        Connections {
            target: root
            onReadyChanged: {
                if(!ready)
                    interfaceLoader.source = "";

                SofaApplication.trimComponentCache();
            }
        }

        property var sofaScene: root
    }

    property Timer canvasLoaderTimer: Timer {
        id: loadCanvasesTimer
        interval: 200
        repeat: true
        running: true
        onTriggered: {
            checkForCanvases()
        }
    }

}

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
import QtQuick.Controls 1.0
import SofaApplication 1.0
import SofaParticleInteractor 1.0
import SofaScene 1.0

SofaScene {
    id: root

    asynchronous: true
    header: ""
    source: ""
    sourceQML: ""
    property string statusMessage: ""

    Component.onCompleted: {
        SofaApplication.sofaScene = root;
    }

    Component.onDestruction: {
        if(root === SofaApplication.sofaScene)
            SofaApplication.sofaScene = null;
    }

    onStatusChanged: {
        switch(status) {
        case SofaScene.Loading:
            statusMessage = 'SofaScene loading "' + root.path + '" please wait';
            break;
        case SofaScene.Error:
            statusMessage = 'SofaScene "' + root.path + '" issued an error during loading';
            break;
        case SofaScene.Ready:
            statusMessage = 'SofaScene "' + root.path + '" loaded successfully';
            SofaApplication.sceneSettings.addRecent(root.path);
            break;
        }
    }

    // convenience
    readonly property bool ready: status === SofaScene.Ready

    // allow us to interact with the python script controller
    property var sofaPythonInteractor: SofaPythonInteractor {sofaScene: root}

    // allow us to interact with the sofa scene particles
    property var sofaParticleInteractor: SofaParticleInteractor {
        stiffness: 100

        onInteractingChanged: SofaApplication.overrideCursorShape = interacting ? Qt.BlankCursor : 0
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

    // SOFACOMPONENT

    /// \note you can use data with a SofaComponent and fill the name of the data or directly put the data path in the data field and not use the name parameter
    function dataValue(data, name) {
        if(!Qt.isQtObject(data))
            return onDataValueByPath(data);
        else
            return onDataValueByComponent(data, name);

        console.debug("ERROR: SofaScene - using dataValue with an invalid number of arguments:", arguments.length);
    }

    /// \note you can use data with a SofaComponent and fill the name of the data or directly put the data path in the data field and not use the name parameter (just ignore it), the other parameters will be taken as the values to set
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

        console.debug("ERROR: SofaScene - using setDataValue with an invalid number of arguments:", arguments.length);
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

        // delay the load of the interface to one frame to let the cache be trimmed in order to load the scene interface from scratch without reusing the previous loaded one in case of a scene reloading
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
}

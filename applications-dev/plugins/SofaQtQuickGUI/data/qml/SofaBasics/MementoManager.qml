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

pragma Singleton
import QtQuick 2.0
import "."

QtObject {
    id: root

    property bool enabled: !SofaApplication.sofaScene || SofaApplication.sofaScene.ready || SofaScene.Null === SofaApplication.sofaScene.status

    readonly property alias currentSnapshotId: d.currentSnapshotId
    readonly property bool undoReady: enabled && d.currentSnapshotId > 0
    readonly property bool redoReady: enabled && d.currentSnapshotId < d.snapshots.length - 1
    readonly property string undoActionName: snapshotGotoMessage(undoReady ? currentSnapshotId - 1 : -1)
    readonly property string redoActionName: snapshotGotoMessage(redoReady ? currentSnapshotId + 1 : -1)
    readonly property alias snapshots: d.snapshots

    function addMementoObject(object) {
        d.mementoObjects[object.level] = object;
    }

    function removeMementoObject(object) {
        if(object === d.mementoObjects[object.level])
            delete d.mementoObjects[object.level];
    }

    function snapshotGotoMessage(snapshotId) {
        if(snapshotId < 0 || snapshotId >= snapshots.length)
            return "";

        var snapshot = snapshots[snapshotId];

        var dateMessage = " at " + snapshot.date.toTimeString();

        if(snapshotId !== currentSnapshotId) {
            if(snapshots.length - 1 === snapshotId && snapshots.length !== currentSnapshotId)
                return "Goto : \"" + snapshot.actionName + "\"" + dateMessage
            else
                return "Go before: \"" + snapshot.actionName + "\"" + dateMessage;
        }

        return "Currently before: \"" + snapshot.actionName + "\"" + dateMessage;
    }

    function undoAction() {
        if(!undoReady)
            return;

        //console.log("Undo request");

        loadSnapshot(d.currentSnapshotId - 1);
    }

    function redoAction() {
        if(!redoReady)
            return;

        //console.log("Redo request");

        loadSnapshot(d.currentSnapshotId + 1);
    }

    function loadSnapshot(snapshotId) {
        if(loadSnapshot.locked)
            return;

        if(!enabled)
            return;

        if(snapshotId < 0 || snapshotId >= d.snapshots.length || snapshotId === d.currentSnapshotId)
            return;

        if(d.snapshots.length === d.currentSnapshotId)
            saveSnapshot("Final state");

        loadSnapshot.locked = true;

        d.currentSnapshotId = snapshotId;

        var snapshot = d.snapshots[d.currentSnapshotId];

        //console.log("Loading Snapshot n°", snapshotManager.currentSnapshotId, " to action: '", snapshot.actionName, "'")

        for(var i = 0; i < d.mementoObjects.length; ++i) {
            var object = d.mementoObjects[i];
            if(!object)
                continue;

            object.setProperties(snapshot.levelData[i]);
        }

//        applicationStateMachine.patientId = snapshot.patientId;
//        applicationStateMachine.forceGotoStateByName(snapshot.stateName);
//        setStateContentSnapshotData(snapshot.stateContentData);

//        if(SofaApplication.sofaScene && undefined !== snapshot.sceneData)
//            SofaApplication.sofaScene.snapshotData = snapshot.sceneData;

        loadSnapshot.locked = false;
    }

    function saveSnapshot(actionName) {
        if(loadSnapshot.locked)
            return;

        if(!enabled)
            return;

        //console.log("Saving Snapshot n°", snapshotManager.currentSnapshotId, " at action: '", actionName, "'")

        if(d.currentSnapshotId !== d.snapshots.length)
            d.snapshots = d.snapshots.slice(0, d.currentSnapshotId);

        var snapshot = {};

        snapshot.date = new Date();
        snapshot.actionName = actionName;
        snapshot.levelData = [];

        for(var i = 0; i < d.mementoObjects.length; ++i) {
            var object = d.mementoObjects[i];
            if(!object)
                continue;

            snapshot.levelData[i] = object.getProperties();
        }

//        snapshot.patientId = applicationStateMachine.patientId;
//        snapshot.stateName = applicationStateMachine.currentState.name;
//        snapshot.stateContentData = getStateContentSnapshotData();

//        if(SofaApplication.sofaScene)
//            snapshot.sceneData = SofaApplication.sofaScene.downloadSnapshotData();

        d.snapshots.push(snapshot);
        d.snapshotsChanged();

        d.currentSnapshotId = d.snapshots.length;
    }

    property QtObject d: QtObject {
        id: d

        property var mementoObjects: []

        property int currentSnapshotId: 0
        property var snapshots: []
    }
}

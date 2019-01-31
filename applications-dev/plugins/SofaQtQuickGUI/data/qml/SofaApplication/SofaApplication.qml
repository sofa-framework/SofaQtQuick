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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Qt.labs.settings 1.0
import Qt.labs.folderlistmodel 2.1
import SofaApplicationSingleton 1.0
import SofaMessageList 1.0

SofaApplication
{
    id: root

    property var style : MainStyle

    ////////////////////////////////////////////////// SOFASCENE
    property var sofaScene: null
    property QtObject sofaMessageList : SofaMessageList

    property var sceneSettings: Settings {
        category: "scene"

        property string sofaSceneRecents      // recently opened sofa scenes

        function addRecent(path) {
            sofaSceneRecents = path + ";" + sofaSceneRecents.replace(path + ";", "");
        }

        function mostRecent() {
            var recent = sofaSceneRecents.replace(/;.*$/m, "");
            if(0 === recent.length)
                return "";

            return "file:" + recent;
        }

        function recentsList() {
            return sofaSceneRecents.split(';');
        }

        function clearRecents() {
            sofaSceneRecents = "";
        }
    }

    ////////////////////////////////////////////////// TOOLBAR

    property var toolBar: null

    ////////////////////////////////////////////////// STATUSBAR

    property var statusBar: null

    ////////////////////////////////////////////////// SOFAVIEWER

    readonly property alias focusedSofaViewer: viewerPrivate.focusedSofaViewer
    readonly property alias sofaViewers: viewerPrivate.sofaViewers

    function setFocusedSofaViewer(sofaViewer) {
        viewerPrivate.focusedSofaViewer = sofaViewer;
    }

    function addSofaViewer(sofaViewer) {
        viewerPrivate.sofaViewers.push(sofaViewer);

        viewerPrivate.sofaViewersChanged(viewerPrivate.sofaViewers);
    }

    function removeSofaViewer(sofaViewer) {
        var index = viewerPrivate.sofaViewers.indexOf(sofaViewer);
        if(-1 === index)
            return;

        viewerPrivate.sofaViewers.splice(index, 1);

        viewerPrivate.sofaViewersChanged(viewerPrivate.sofaViewers);
    }

    function callOnAllSofaViewers(visitor) {
        for (var i = 0; i < sofaViewers.length; i++)
            visitor(sofaViewers[i]);
    }

    property QtObject _viewerPrivate: QtObject {
        id: viewerPrivate

        property var focusedSofaViewer: null
        property var sofaViewers: []

    }

    ////////////////////////////////////////////////// CAMERA

    function retrieveAllSofaViewerCameras() {
        var cameras = [];

        for (var i = 0; i < sofaViewers.length; i++) {
            cameras.push(sofaViewers[i].camera);
        }

        return cameras;
    }

    function callOnAllCameras(visitor) {
        for (var i = 0; i < sofaViewers.length; i++)
            visitor(sofaViewers[i].camera);
    }

    ////////////////////////////////////////////////// SETTINGS
    /// This stores all the created view in the UI.
    property var uiSettings: Settings {
        category: "ui"

        property string uiIds: ";"

        function generate() {
            var uiId = 1;
            var uiIdList = uiIds.split(";");

            var previousId = 0;
            for(var i = 0; i < uiIdList.length; ++i) {
                if(0 === uiIdList[i].length)
                    continue;

                uiId = Number(uiIdList[i]);

                if(previousId + 1 !== uiId) {
                    uiId = previousId + 1;
                    break;
                }

                previousId = uiId;
                ++uiId;
            }

            return uiId;
        }

        function add(uiId) {
            if(0 === uiId)
                return;

            if(-1 === uiIds.search(";" + uiId.toString() + ";")) {
                uiIds += uiId.toString() + ";";
                update();
            }
        }

        function remove(uiId) {
            if(0 === uiId)
                return;

            uiIds = uiIds.replace(";" + uiId.toString() + ";", ";");

            root.clearSettingGroup("ui_" + uiId.toString());
        }

        function replace(previousUiId, uiId) {
            if(0 === uiId)
                return;

            uiIds = uiIds.replace(";" + uiId.toString() + ";", ";");

            if(-1 === uiIds.search(";" + uiId.toString() + ";")) {
                uiIds += uiId.toString() + ";";
                update();
            }
        }

        function update() {
            var uiIdList = uiIds.split(";");
            uiIdList.sort(function(a, b) {return Number(a) - Number(b);});

            uiIds = ";";
            for(var i = 0; i < uiIdList.length; ++i)
                if(0 !== uiIdList[i].length)
                    uiIds += uiIdList[i] + ";";
        }
    }

    ////////////////////////////////////////////////// INTERACTOR

    property string defaultInteractorName: "SofaParticleInteractor"
    readonly property string interactorName: {
        if(interactorComponent)
        for(var key in interactorComponentMap)
        if(interactorComponentMap.hasOwnProperty(key) && interactorComponent === interactorComponentMap[key])
        return key;

        return "";
    }

    property Component interactorComponent: null
    property var interactorComponentMap: []

    function getInteractorName(component) {
        for(var key in interactorComponentMap)
            if(interactorComponentMap.hasOwnProperty(key))
                if(component === interactorComponentMap[key])
                    return key;

        return "";
    }

    function addInteractor(name, component) {
        interactorComponentMap[name] = component;

        interactorComponentMapChanged(interactorComponentMap);
    }

    function removeInteractor(component) {
        for(var key in interactorComponentMap) {
            if(interactorComponentMap.hasOwnProperty(key)) {
                if(component === interactorComponentMap[key]) {
                    delete interactorComponentMap[key];

                    interactorComponentMapChanged(interactorComponentMap);

                    return;
                }
            }
        }
    }

    function removeInteractorByName(name) {
        var component = interactorComponentMap[name];
        if(!component)
            return;

        delete interactorComponentMap[name];

        interactorComponentMapChanged(interactorComponentMap);
    }

    function clearInteractors() {
        interactorComponentMap = [];
    }

    // init interactorComponentMap at startup
    property var interactorFolderListModel: FolderListModel {
        id: interactorFolderListModel
        nameFilters: ["*.qml"]
        showDirs: false
        showFiles: false
        sortField: FolderListModel.Name
        folder: "qrc:/SofaInteractors"

        property var delayedUpdateTimer: Timer {
            interval: 1
            repeat: false
            running: true
            onTriggered: interactorFolderListModel.showFiles = true;
        }

        onCountChanged: update();

        property var sceneConnections: Connections {
            target: root.sofaScene
            onReadyChanged: if(root.sofaScene.ready) interactorFolderListModel.refresh();
        }

        function refresh() {
            showFiles = false;
            showFiles = true;
        }

        function update() {
            root.clearInteractors();

            var interactorComponentMap = [];
            for(var i = 0; i < count; ++i)
            {
                var fileBaseName = get(i, "fileBaseName");
                var filePath = get(i, "filePath").toString();
                if(-1 !== folder.toString().indexOf("qrc:"))
                    filePath = "qrc" + filePath;

                var name = fileBaseName.slice(fileBaseName.indexOf("_") + 1);
                var interactorComponent = Qt.createComponent(filePath);
                interactorComponentMap[name] = interactorComponent;
            }

            if(null === root.interactorComponent)
                if(interactorComponentMap.hasOwnProperty(root.defaultInteractorName))
                    root.interactorComponent = interactorComponentMap[root.defaultInteractorName];

            if(null === root.interactorComponent)
                for(var key in interactorComponentMap)
                    if(interactorComponentMap.hasOwnProperty(key)) {
                        root.interactorComponent = interactorComponentMap[key];
                        break;
                    }

            root.interactorComponentMap = interactorComponentMap;
        }
    }

    ////////////////////////////////////////////////// SCREENSHOT

    function formatDateForScreenshot() {
        var today = new Date();
        var day = today.getDate();
        var month = today.getMonth();
        var year = today.getFullYear();

        var hour = today.getHours();
        var min = today.getMinutes() + hour * 60;
        var sec = today.getSeconds() + min * 60;
        var msec = today.getMilliseconds() + sec * 1000;

        return year + "-" + month + "-" + day + "_" + msec;
    }

    function takeScreenshot(savePath) {
        if(undefined === savePath)
            savePath = "Captured/Screen/" + root.formatDateForScreenshot() + ".png";

        if(-1 === savePath.lastIndexOf("."))
            savePath += ".png";

        root.saveScreenshot(savePath);
    }

    function startVideoRecording(savePath) {
        videoRecordingPrivate.videoPath = savePath;
        videoRecordingPrivate.saveVideo = true;
    }

    function stopVideoRecording() {
        videoRecordingPrivate.saveVideo = false;
    }

    property var _videoRecordingPrivate: QtObject {
        id: videoRecordingPrivate

        property string videoPath: ""
        property bool saveVideo: false
        onSaveVideoChanged: {
            if(!saveVideo)
            return;

            videoFrameNumber = 0;

            saveVideoFrame();
        }

        property int videoFrameNumber: 0
        property var sceneConnections: Connections {
            target: root.sofaScene && videoRecordingPrivate.saveVideo ? root.sofaScene : null
            onStepEnd: videoRecordingPrivate.saveVideoFrame();
        }

        function saveVideoFrame() {
            var savePath = videoRecordingPrivate.videoPath;
            if(0 === savePath.length)
                savePath = "Captured/Movie/" + root.formatDateForScreenshot() + "/movie.png";

            var dotIndex = savePath.lastIndexOf(".");
            if(-1 === dotIndex) {
                savePath += ".png"
                dotIndex = savePath.lastIndexOf(".");
            }

            savePath = savePath.slice(0, dotIndex) + "_" + (videoRecordingPrivate.videoFrameNumber++).toString() + savePath.slice(dotIndex);

            root.saveScreenshot(savePath);
        }
    }

    ////////////////////////////////////////////////// MISC

    function urlToPath(url) {
        return  Qt.platform.os === "windows" ? Qt.resolvedUrl(url).toString().replace("file:///", "") : Qt.resolvedUrl(url).toString().replace("file://", "")
    }

    //////////////////////////////////////////////////

}

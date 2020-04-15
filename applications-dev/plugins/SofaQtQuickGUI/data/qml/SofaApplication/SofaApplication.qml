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
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Qt.labs.settings 1.0
import Qt.labs.folderlistmodel 2.1
import SofaBaseApplicationSingleton 1.0
import SofaMessageList 1.0
import SofaViewListModel 1.0
import SofaProject 1.0
//import GraphView 1.0
import SofaBaseScene 1.0
import SofaInteractors 1.0
import SofaBasics 1.0

Item //
{
    id: root

    ////////////////////////////////////////////////// SOFASCENE
    property var sofaScene: null

    property QtObject sofaMessageList : SofaMessageList

    property var style : MainStyle

    ////////////////////////////////////////////////// SELECTED COMPONENT
    property var selectedComponent : SofaBaseApplicationSingleton.selectedComponent
    onSelectedComponentChanged: {
        if (SofaBaseApplicationSingleton.selectedComponent !== selectedComponent)
            SofaBaseApplicationSingleton.selectedComponent = selectedComponent
    }

    /// Connect to this signal to be notified when a component need to be emphasized.
    signal signalComponent(string objectpath)

    function trimComponentCache()
    {
        SofaBaseApplicationSingleton.trimComponentCache();
    }

    function openInEditor(fullpath, line)
    {
        SofaBaseApplicationSingleton.openInEditor(fullpath, line)
    }

    function openInEditorFromUrl(urlToPath, line)
    {
        SofaBaseApplicationSingleton.openInEditorFromUrl(urlToPath, line)
    }

    function openInExplorer(fullpath)
    {
        SofaBaseApplicationSingleton.openInExplorer(fullpath)
    }

    function openInTerminal(fullpath)
    {
        SofaBaseApplicationSingleton.openInTerminal(fullpath)
    }

    function copyFile(src, dest)
    {
        SofaBaseApplicationSingleton.copyFile(src, dest)
    }

    /// Returns the absolute position of the mouse in a mouseArea
    /// Takes a MouseArea as argument
    function getAbsolutePosition(node) {
        var returnPos = _getAbsolutePosition(node)
        returnPos.x += node.mouseX
        returnPos.y += node.mouseY
        return returnPos;
    }

    function _getAbsolutePosition(node) {
        var returnPos = {};
        returnPos.x = 0;
        returnPos.y = 0;
        if(node !== undefined && node !== null) {
            var parentValue = _getAbsolutePosition(node.parent);
            returnPos.x = parentValue.x + node.x;
            returnPos.y = parentValue.y + node.y;
        }

        return returnPos;
    }

    /// Returns the dimensions of the top parent window
    /// Takes a MouseArea as argument
    function getWindowDimensions(node) {
        var returnPos = {};
        if (node === undefined || node === null) {
            return returnPos
        }

        if (node.parent === undefined || node.parent === null) {
            returnPos.x = node.width;
            returnPos.y = node.height;
            return returnPos;
        }
        return getWindowDimensions(node.parent);
    }

    /// Returns the ideal positioning of a popup, relative to the mouse's position
    function getIdealPopupPos(popup, mouseArea) {
        var absPos = getAbsolutePosition(mouseArea)
        var absDim = getWindowDimensions(mouseArea)
        if (absPos.x + popup.width < absDim.x) {
            if (absPos.y + popup.height < absDim.y) {
                return [0, 0]
            } else {
                return [0, -popup.height]
            }
        } else {
            if (absPos.y + popup.height < absDim.y) {
                return [-popup.width, 0]
            } else {
                return [-popup.width, -popup.height]
            }
        }
    }

    property var nodeSettings: Settings {
        category: "Hierarchy"
        property string nodeState: ""
    }


    ////////////////////////////////////////////////// PROJECTSETTINGS
    property var currentProject : SofaProject {
        property var selectedAsset: null
        rootDir: projectSettings.currentProject()

        scene: sofaScene
        Component.onCompleted: {
            SofaBaseApplicationSingleton.currentProject = currentProject;
        }
    }

    property var projectSettings: Settings {
        id: projectSettings
        category: "projects"
        property string recentProjects

        function addRecent(path) {
            path = path.toString().replace("file://", "").replace("qrc:", "")
            recentProjects = path + ";" + recentProjects.replace(path + ";", "");
        }
        function currentProject() {
            var recentsList = recentProjects.split(";")
            if (recentsList === [])
                return ""
            return recentsList[0]
        }
    }


    property var sceneSettings: Settings {
        category: "scene"

        property string sofaSceneRecents      // recently opened sofa scenes

        function addRecent(path) {
            path = path.toString().replace("file://", "").replace("qrc:", "")
            sofaSceneRecents = path + ";" + sofaSceneRecents.replace(path + ";", "");
        }

        function mostRecent() {
            var recent = sofaSceneRecents.replace(/;.*$/m, "");
            if(0 === recent.length)
                return "";

            return "file:" + recent;
        }
    }

    ////////////////////////////////////////////////// VIEWS MODEL
    property var sofaViewListModel: SofaViewListModel
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

    property UserInteractor interactorComponent


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

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
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.3
import SofaBasics 1.0
import SofaApplication 1.0
import SofaTools 1.0
import SofaWidgets 1.0
import "qrc:/SofaCommon/SofaSettingsScript.js" as SofaSettingsScript

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    title: Qt.application.name + " - \"" + scenePath + "\""

    property var scene: Scene {
        Component.onCompleted: {
            if(Qt.application.arguments.length > 1) {
                scene.source = "file:" + Qt.application.arguments[1];
            }
            else {
                if(0 !== SofaSettingsScript.Recent.scenes.length)
                    scene.source = SofaSettingsScript.Recent.mostRecent();
                else
                    scene.source = "file:Demos/caduceus.scn";
            }
            scenePath = scene.source.toString().replace("///", "/").replace("file:", "");
        }
    }

    property string scenePath: ""

    style: ApplicationWindowStyle {
        background: null
    }

    Component.onCompleted: {
        showNormal();
        //showFullScreen();
    }

    // dialog
    property FileDialog openSofaSceneDialog: openSofaSceneDialog
    FileDialog {
        id: openSofaSceneDialog
        nameFilters: ["Scene files (*.xml *.scn *.pscn *.py *.simu *)"]
        onAccepted: {
            scene.source = fileUrl;
            scenePath = scene.source.toString().replace("///", "/").replace("file:", "");
        }
    }

    property FileDialog saveSofaSceneDialog: saveSofaSceneDialog
    FileDialog {
        id: saveSofaSceneDialog
        selectExisting: false
        nameFilters: ["Scene files (*.scn)"]
        onAccepted: {
            scene.save(fileUrl);
        }
    }

    menuBar: DefaultMenuBar {
        id: menuBar
        scene: root.scene
    }

    toolBar: DefaultToolBar {
        id: toolBar
        scene: root.scene
    }

    DynamicSplitView {
        id: dynamicSplitView
        anchors.fill: parent
        uiId: 1
        sourceComponent: Component {
            DynamicContent {
                defaultContentName: "Viewer"
                sourceDir: "qrc:/SofaWidgets"
                properties: {"scene": root.scene}
            }
        }
    }

    statusBar: DefaultStatusBar {
        id: statusBar
        scene: root.scene
    }
}

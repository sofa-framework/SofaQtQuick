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
import SofaScene 1.0
import SofaBasics 1.0
import SofaApplication 1.0
import SofaViews 1.0

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    title: Qt.application.name + " - \"" + sofaScene.path + "\""

    onClosing: Qt.quit()

    property var sofaScene: SofaScene {
        id: sofaScene

        // delay the opening of the previous scene to the next frame to let a chance to parse command line arguments specifying another scene
        property var openPreviousTimer: Timer {
            running: true
            repeat: false
            interval: 1

            onTriggered: {
                if(SofaScene.Null !== sofaScene.status)
                    return;

                var source = SofaApplication.sceneSettings.mostRecent();
                if(0 === source.length)
                    source = "file:Demos/caduceus.scn";

                sofaScene.source = source;
            }
        }
    }

//    style: ApplicationWindowStyle {
//        background: null
//    }

    Component.onCompleted: {
        showNormal();
        //showFullScreen();
    }

    // dialog
    property FileDialog openSofaSceneDialog: openSofaSceneDialog
    FileDialog {
        id: openSofaSceneDialog
        nameFilters: ["SofaScene files (*.xml *.scn *.pscn *.py *.simu *)"]
        onAccepted: sofaScene.source = fileUrl;
    }

    menuBar: DefaultMenuBar {
        id: menuBar
        sofaScene: root.sofaScene
    }

    toolBar: DefaultToolBar {
        id: toolBar
        sofaScene: root.sofaScene
    }

    DynamicSplitView {
        id: dynamicSplitView
        anchors.fill: parent
        uiId: 1
        sourceComponent: Component {
            DynamicContent {
                defaultContentName: "SofaViewer"
                sourceDir: "qrc:/SofaViews"
                properties: {"sofaScene": root.sofaScene, "drawFrame": false}
            }
        }
    }

    statusBar: DefaultStatusBar {
        id: statusBar
        sofaScene: root.sofaScene
    }
}

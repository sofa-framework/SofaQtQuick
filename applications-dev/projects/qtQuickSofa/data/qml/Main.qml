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

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    title: Qt.application.name + " - \"" + sofaScenePath + "\""

    onClosing: Qt.quit()

    property var sofaScene: SofaScene {
        Component.onCompleted: {
            if(Qt.application.arguments.length > 1) {
                sofaScene.source = "file:" + Qt.application.arguments[1];
            }
            else {
                var source = SofaApplication.sceneSettings.mostRecent();
                if(0 === source.length)
                    source = "file:Demos/caduceus.scn";

                sofaScene.source = source;
            }
            sofaScenePath = sofaScene.source.toString().replace("///", "/").replace("file:", "");
        }
    }

    property string sofaScenePath: ""

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
        nameFilters: ["SofaScene files (*.xml *.scn *.pscn *.py *.simu *)"]
        onAccepted: {
            sofaScene.source = fileUrl;
            sofaScenePath = sofaScene.source.toString().replace("///", "/").replace("file:", "");
        }
    }

    property FileDialog saveSofaSceneDialog: saveSofaSceneDialog
    FileDialog {
        id: saveSofaSceneDialog
        selectExisting: false
        nameFilters: ["SofaScene files (*.scn)"]
        onAccepted: {
            sofaScene.save(fileUrl);
        }
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
                sourceDir: "qrc:/SofaWidgets"
                properties: {"sofaScene": root.sofaScene}
            }
        }
    }

    statusBar: DefaultStatusBar {
        id: statusBar
        sofaScene: root.sofaScene
    }
}

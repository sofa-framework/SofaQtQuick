/*
Copyright 2015, Anatoscope
Copyright 2017, CNRS

This file is part of runSofa2

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

contributors:
    - damien.marchal@univ-lille1.fr
*/

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.3
import SofaScene 1.0
import SofaBasics 1.0
import SofaApplication 1.0
import RS2Application 1.0
import SofaWidgets 1.0

ApplicationWindow {
    id: root
    width: RS2Application.defaultWidth
    height: RS2Application.defaultHeight
    visibility: RS2Application.defaultVisibility
    visible: true

    title: Qt.application.name + " - \"" + sofaScene.path + "\""

    onClosing:
        Qt.quit()

    property var sofaScene: SofaScene {
        id: sofaScene
        source: RS2Application.defaultScene
        defaultAnimate: RS2Application.animateOnLoad
    }

    Component.onCompleted: {
        showNormal();
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
                sourceDir: "qrc:/SofaWidgets"
                properties: {"sofaScene": root.sofaScene, "drawFrame": false}
            }
        }
    }

    statusBar: DefaultStatusBar {
        id: statusBar
        sofaScene: root.sofaScene
    }
}

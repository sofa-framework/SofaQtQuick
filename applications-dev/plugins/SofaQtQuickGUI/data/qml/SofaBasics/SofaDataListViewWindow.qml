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
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaApplication 1.0
import SofaComponent 1.0
import SofaDataListModel 1.0
import SofaWidgets 1.0

Window {
    id: root
    width: 400
    height: 600
    modality: Qt.NonModal
    flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
    visible: true
    color: "lightgrey"

    Component.onCompleted: {
        height = Math.min(height, listView.contentHeight);
    }

    title: sofaComponent ? ("Data of component: " + sofaComponent.name()) : "No component to visualize"

    property var sofaScene: SofaApplication.sofaScene
    property var sofaComponent: sofaScene ? sofaScene.selectedComponent : null

    Loader {
        anchors.fill: parent

        sourceComponent: SofaDataListView {
            id: listView

            sofaScene: root.sofaScene
            sofaComponent: root.sofaComponent
        }
    }
}

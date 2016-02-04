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

Window {
    id: root
    width: 400
    height: Math.min(600, listView.contentHeight)
    modality: Qt.NonModal
    flags: Qt.Dialog | Qt.WindowStaysOnTopHint
    visible: true
    color: "lightgrey"

    title: sofaComponent ? ("Data of component: " + sofaComponent.name()) : "No component to visualize"

    property var sofaScene: SofaApplication.sofaScene
    property var sofaComponent: sofaScene ? sofaScene.selectedComponent : null

    ScrollView {
        id: scrollView
        anchors.fill: parent
        enabled: sofaScene && sofaComponent ? sofaScene.ready : false
        frameVisible: true

        SofaDataListViewItem {
            id: listView
            anchors.fill: parent
            anchors.rightMargin: 8
            focus: true

            sofaScene: root.sofaScene
            sofaComponent: root.sofaComponent
        }
    }
}

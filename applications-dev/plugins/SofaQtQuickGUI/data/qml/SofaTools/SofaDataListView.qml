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
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import SofaComponent 1.0
import SofaDataListModel 1.0

CollapsibleGroupBox {
    id: root
    implicitWidth: 0

    contentRightMargin: 9
    title: "Data"
    property int priority: 80
    contentMargin: 0

    property var sofaScene: SofaApplication.sofaScene
    property var sofaComponent: !root.collapsed && sofaScene ? sofaScene.selectedComponent : null

    enabled: sofaScene ? sofaScene.ready : false

    GridLayout {
        id: layout
        anchors.fill: parent
        columns: 1

        SofaDataListViewItem {
            id: listView
            Layout.fillWidth: true
            Layout.preferredHeight: contentHeight
            focus: true

            sofaScene: root.sofaScene
            sofaComponent: root.sofaComponent
        }
    }
}

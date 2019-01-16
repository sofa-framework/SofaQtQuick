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
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaDataTypes 1.0

ColumnLayout {
    id: root
    spacing: 0
    enabled: sofaScene ? sofaScene.ready : false

    width: 300
    height: sofaDataWidget.implicitHeight

    property var sofaScene: SofaApplication.sofaScene

    ScrollView {
        id: scrollView
        Layout.fillWidth: true
        Layout.preferredHeight: Math.min(root.height, sofaDataWidget.implicitHeight)

        SofaDataType_widget_displayFlags {
            id: sofaDataWidget
            implicitWidth: scrollView.width

            Component.onCompleted: updateVisualStyle();
            onSceneReadyChanged: updateVisualStyle();

            property bool sceneReady: root.sofaScene && root.sofaScene.ready

            function updateVisualStyle() {
                dataObject = null;
                if(sceneReady) {
                    var visualStyleComponent = root.sofaScene.visualStyleComponent();
                    if(visualStyleComponent)
                        dataObject = visualStyleComponent.getComponentData("displayFlags").object();
                }
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

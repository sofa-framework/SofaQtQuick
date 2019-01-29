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
import SofaBasics 1.0
import SofaApplication 1.0

ColumnLayout {
    id: root
    spacing: 0
    enabled: sofaScene ? sofaScene.ready : false

    width: 300
    height: columnLayout.implicitHeight

    property var sofaScene: SofaApplication.sofaScene

    readonly property Loader interfaceLoader: sofaScene ? sofaScene.interfaceLoader : null
    readonly property QtObject interfaceObject: interfaceLoader ? interfaceLoader.item : null
    readonly property Component interfaceToolPanel: interfaceObject ? interfaceObject.toolpanel : null

    ScrollView {
        id: scrollView
        Layout.fillWidth: true
        Layout.preferredHeight: Math.min(root.height, columnLayout.implicitHeight)

        ColumnLayout {
            id: columnLayout
            width: scrollView.width
            spacing: 0

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: loader.height

                Loader {
                    id: loader
                    asynchronous: true
                    sourceComponent: root.interfaceToolPanel
                    width: parent.width
                    height: Loader.Ready === status ? implicitHeight : 0
                    visible: Loader.Ready === status
                }
            }

            Label {
                id: errorLabel
                Layout.fillWidth: true
                Layout.preferredHeight: implicitHeight
                visible: Loader.Error === loader.status
                textFormat: Text.RichText

                text: "<b><font color='darkred'>Error loading sofa scene toolpanel, take a look in the console for error log</font></b>"
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }

            Label {
                id: emptyLabel
                Layout.fillWidth: true
                Layout.preferredHeight: implicitHeight
                visible: Loader.Null === loader.status

                text: "This scene has no graphical user interface"
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }

            BusyIndicator {
                id: busyIndicator
                visible: running
                Layout.fillWidth: true
                Layout.fillHeight: true
                running: Loader.Loading === loader.status
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

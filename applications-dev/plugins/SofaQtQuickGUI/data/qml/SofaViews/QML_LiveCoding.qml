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
import QtQuick.Dialogs 1.1
import SofaBasics 1.0
import SofaApplication 1.0

SplitView {
    id: root
    orientation: Qt.Horizontal

    SplitView {
        id: scriptLayout
        Layout.fillWidth: true
        Layout.minimumWidth: 64
        Layout.fillHeight: true
        orientation: Qt.Vertical

        TextArea {
            id: scriptTextArea
            Layout.fillWidth: true
            Layout.fillHeight: true

            onTextChanged: reloadTimer.restart()

            text: "import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0

Rectangle {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true

    color: 'black'
}"
        }

        TextArea {
            id: consoleTextArea
            Layout.fillWidth: true
            enabled: true

            textFormat: TextEdit.RichText
            textColor: "darkgrey"
        }
    }

    Item {
        id: container
        implicitWidth: root.width / 2
        Layout.fillHeight: true

        Component.onCompleted: reload();

        Timer {
            id: reloadTimer
            running: false
            repeat: false
            interval: 200

            onTriggered: container.reload();
        }

        property Item item: null
        function reload() {
            var script = scriptTextArea.text;

            if(item)
                item.destroy();

            consoleTextArea.text = "";
            try {
                item = Qt.createQmlObject(script, container, "livecoding");
            }
            catch(exception) {
                var qmlErrors = exception.qmlErrors;
                for(var i = 0 ; i < qmlErrors.length; ++i) {
                    var qmlError = qmlErrors[i];

                    // TODO: add link to error line
                    consoleTextArea.text = "Error at " + qmlError.lineNumber + ":" + qmlError.columnNumber + ": <br />" + qmlError.message + "<br />";
                }
            }

            if(item)
                item.anchors.fill = container;
        }
    }
}

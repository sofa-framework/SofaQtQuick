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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import SofaBasics 1.0
import SofaApplication 1.0
import SofaScene 1.0
import PythonConsole 1.0

ColumnLayout {
    id: root
    spacing: 0

    property var sofaScene: SofaApplication.sofaScene

    TextArea {
        id: consoleTextArea
        Layout.fillWidth: true
        Layout.fillHeight: true
        readOnly: true

        onTextChanged: cursorPosition = Math.max(0, text.length - 1)

        Connections {
            target: root.sofaScene
            onAboutToUnload: consoleTextArea.text = ""
        }

        PythonConsole {
            onTextAdded: consoleTextArea.text += text
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: 24

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 32
            spacing: 0

            TextField {
                id: scriptTextField
                Layout.fillWidth: true
                onAccepted: run();

                property int indexHistory: 0
                property int maxHistory: 100
                property var commandHistory: ["", ""]
                property bool lock: false

                onTextChanged: {
                    if(0 === indexHistory || !lock)
                        commandHistory[0] = text;

                    if(!lock)
                        indexHistory = 0;
                }

                Keys.onUpPressed: {
                    if(commandHistory.length - 1 === indexHistory)
                        return;

                    indexHistory = Math.min(++indexHistory, commandHistory.length - 1);

                    lock = true;
                    text = commandHistory[indexHistory];
                    lock = false;
                }

                Keys.onDownPressed: {
                    if(0 === indexHistory)
                        return;

                    indexHistory = Math.max(--indexHistory, 0);
                    lock = true;
                    text = commandHistory[indexHistory];
                    lock = false;
                }

                function run() {
                    if(0 === text.length)
                        return;

                    sofaScene.sofaPythonInteractor.run(text);

                    if(0 !== text.localeCompare(commandHistory[1])) {
                        commandHistory[0] = text;
                        commandHistory.splice(0, 0, "");
                        if(commandHistory.length > maxHistory)
                            commandHistory.length = maxHistory;
                    }

                    indexHistory = 0;
                    text = "";
                }
            }

            Button {
                text: "Run"
                onClicked: scriptTextField.run();
            }
        }
    }
}

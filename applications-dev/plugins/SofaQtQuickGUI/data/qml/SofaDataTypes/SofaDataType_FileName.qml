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

import QtQuick 2.2
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.0
import SofaBasics 1.0
import Sofa.Core.SofaDataFileName 1.0
import SofaApplication 1.0

RowLayout {
    id: dataFileName
    property SofaDataFileName sofaData
    anchors.fill: parent
    spacing: -1

    Connections {
        target: sofaData
        onValueChanged: {
            textfield.text = getTextFieldValue(sofaData.getFullPath())
        }
    }

    ImportFileDialog {
        id: importDialog
    }

    function isInProjectTree(path) {
        if (path.startsWith(SofaApplication.currentProject.rootDirPath))
                return true
        else if (SofaApplication.fileExists(SofaApplication.currentProject.rootDirPath + "/" + path))
            return true
        return false;
    }

    function getRelativeToProjectTree(path) {
        if (path.startsWith(SofaApplication.currentProject.rootDirPath))
            return path.replace(SofaApplication.currentProject.rootDirPath, "").slice(1)
        return path
    }

    // converts a path to either relative to root, or absolute if out-of-tree
    function getTextFieldValue(path) {
        if (isInProjectTree(path)) {
            return getRelativeToProjectTree(path)
        }
        return path
    }


    TextField {
        id: textfield
        text: getTextFieldValue(sofaData.getFullPath())

        Layout.fillWidth: true
        enabled: true
        readOnly: sofaData.properties.readOnly
        selectByMouse: true
        position: cornerPositions["Left"]

        function setDataValue(txt) {
            importDialog.externalFile = txt
            if (!isInProjectTree(txt)) {
                importDialog.open()
            }
            sofaData.setValue(importDialog.externalFile)
            focus = false
        }

        onAccepted: {
            setDataValue(text)
        }

        onEditingFinished: {
            setDataValue(text)
        }

        DropArea {
            id: dropArea
            anchors.fill: parent
            onDropped: {
                if (drag.source.asset.path && !textfield.readOnly) {
                    setDataValue(drag.source.asset.path)
                }
            }
        }
    }
    Button {
        id: openButton
        Layout.alignment: Qt.AlignTop
        enabled: !sofaData.properties.readOnly
        Image {
            width: 16
            height: 16
            source: "qrc:/icon/open.png"
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit

        }

        function openDirectoryDialog(dir) {
            var target = SofaApplication.currentProject.chooseProjectDir(
                        "Please Choose a directory:", dir).toString().replace("file://", "")
            if (target !== "") target += "/"
            textfield.setDataValue(target)
        }

        function openFileDialog(dir) {
            var target = SofaApplication.currentProject.getOpenFile("Please Choose a file:", dir).toString().replace("file://", "")
            textfield.setDataValue(target)
        }

        onClicked: {
            var currentDir = SofaApplication.currentProject.rootDir;
            if( sofaData.properties.folderurl !== "")
                currentDir = "file://"+sofaData.properties.folderurl

            if (sofaData.isDirectory())
                openDirectoryDialog(currentDir)
            else
                openFileDialog(currentDir)
        }
        position: cornerPositions["Right"]
    }
}

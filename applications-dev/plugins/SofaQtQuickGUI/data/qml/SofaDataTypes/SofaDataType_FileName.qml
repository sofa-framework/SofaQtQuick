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
import Sofa.Core.SofaData 1.0
import SofaApplication 1.0

/***************************************************************************************************
  *
  * A widget dedicated to edit DataFileName object.
  * This object is used to store filenames.
  *
  *  TextField + Icon to open a file selector.
  *
  *************************************************************************************************/
Row {
    id: root
    spacing : -1
    width: parent.width

    property SofaData sofaData

    function cleanDisplayPath(fileUrl) {
        if (fileUrl === SofaApplication.currentProject.rootDir.toString().replace("qrc:", "") + "/")
            var path = "./"
        else {
            path = fileUrl.replace(SofaApplication.currentProject.rootDir.toString().replace("qrc:", ""), "")
            if (path[0] === "/") {
                path = path.substring(1, path.length)
            }
        }
        return path
    }

    TextField {
        id: textField
        enabled: true
        readOnly: sofaData.properties.readOnly
        width: root.width - openButton.width - root.spacing
        text: cleanDisplayPath(sofaData.value.toString())
        selectByMouse: true

        onAccepted: {
            /// Get the URL from the file chooser and convert it to a string.
            sofaData.value = textField.text ;
        }
        position: cornerPositions["Left"]

        DropArea {
            id: dropArea
            anchors.fill: parent
            onDropped: {
                if (drag.source.asset.path && !textField.readOnly)
                {
                    textField.text = cleanDisplayPath(drag.source.asset.path)
                    sofaData.value = textField.text ;
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

        Dialog {
            id: importAssetDialog
            standardButtons: StandardButton.Yes | StandardButton.No
            title: qsTr("Import asset?")
            width: 400
            height: 150
            contentItem: Rectangle  {
                anchors.fill: parent
                color: SofaApplication.style.contentBackgroundColor
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    TextArea {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: qsTr("The file you selected is located outside of the project.\nThis will break its packageability.\nWould you rather copy the asset to your project directory?")
                    }
                    TextField {
                        id: path
                        Layout.fillWidth: true
                        text: textField.text
                    }
                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            text: ""
                        }
                        Button {
                            id: yes
                            text: "Yes"
                            onClicked: {
                                var filename = textField.text.split("/")
                                filename = filename[filename.length-1]
                                SofaApplication.copyFile(textField.text, path.text + filename)
                                sofaData.value = path.text + filename
                                textField.text = path.text + filename
                                print("copied!")
                                importAssetDialog.close()
                            }
                        }
                        Button {
                            id: no
                            text: "No"
                            onClicked: {
                                sofaData.value = textField.text
                                importAssetDialog.close()
                            }
                        }
                    }
                }
            }
        }
        onClicked: {
            /// Open the FileDialog at the specified location.
            var url = "";
            if( sofaData.properties.folderurl !== ""){
                url = "file://"+sofaData.properties.folderurl
            }else{
                url = "file://"+SofaApplication.currentProject.rootDir
            }
            print (url)
            if (sofaData.isDirectory()) {
                var fileUrl = SofaApplication.currentProject.chooseProjectDir("Please Choose a directory:", url).toString().replace("file://", "")
                if (fileUrl !== "") fileUrl += "/"
            }
            else
                fileUrl = SofaApplication.currentProject.getOpenFile("Please Choose a file:", url).toString().replace("file://", "")
            if (fileUrl === "") return
            sofaData.value = fileUrl;
            print(fileUrl)
            print(SofaApplication.currentProject.rootDir)
            if (fileUrl.startsWith(SofaApplication.currentProject.rootDir.toString().replace("qrc:", ""))) {
                textField.text = cleanDisplayPath(fileUrl)
            } else {
                textField.text = fileUrl
                importAssetDialog.open()
            }
        }
        position: cornerPositions["Right"]
    }
}

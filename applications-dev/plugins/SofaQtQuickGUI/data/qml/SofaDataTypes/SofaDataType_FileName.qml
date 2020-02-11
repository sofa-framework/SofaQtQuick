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

    TextField {
        id: textField
        enabled: true
        readOnly: sofaData.properties.readOnly
        width: root.width - openButton.width - root.spacing
        text: sofaData.value.toString()
        selectByMouse: true

        onAccepted: {
            /// Get the URL from the file chooser and convert it to a string.
            sofaData.value = textField.text ;
        }
        position: cornerPositions["Left"]

        DropArea {
            id: dropArea
            anchors.fill: parent
            onEntered: {
                if(  drag.source.url && !textField.readOnly)
                    drag.accept(false)
            }
            onDropped: {
                if(drag.source.url && !textField.readOnly)
                {
                    textField.text = drag.source.localPath
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
                        text: SofaApplication.currentProject.rootDir.toString().replace("file://", "") + "/assets/resources/"
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
                                print(textField.text + "    " + path.text + filename)
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
                url = SofaApplication.currentProject.rootDir
            }
            if (sofaData.isDirectory()) {
                print("isDirectory: " + sofaData.isDirectory())
                var fileUrl = SofaApplication.currentProject.chooseProjectDir("Please Choose a file:", url)
            }
            else
                fileUrl = SofaApplication.currentProject.getOpenFile("Please Choose a file:", url)
            if (fileUrl.toString() === "") return
            if (fileUrl.toString().startsWith(SofaApplication.currentProject.rootDir)) {
                sofaData.value = fileUrl.toString().replace("file://","");
                textField.text = sofaData.value.toString();
            } else {
                textField.text = fileUrl.toString().replace("file://","")
                importAssetDialog.open()
            }


        }
        position: cornerPositions["Right"]
    }


    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: "file://"+sofaData.properties.folderurl
        onAccepted: {
            /// Get the URL from the file chooser and convert it to a string.
            sofaData.value = fileDialog.fileUrl.toString().replace("file://","") ;
            textField.text = sofaData.value.toString();
        }
    }

}

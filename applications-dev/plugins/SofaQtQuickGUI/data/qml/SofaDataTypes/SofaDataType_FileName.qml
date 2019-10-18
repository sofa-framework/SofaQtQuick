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
import QtQuick.Dialogs 1.0
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
        width: root.width - openButton.width - root.spacing
        text: sofaData.value.toString()

        onAccepted: {
            /// Get the URL from the file chooser and convert it to a string.
            dataObject.value = textField.text ;
        }
        position: cornerPositions["Left"]

        DropArea {
            id: dropArea
            anchors.fill: parent
            onEntered: {
                if(  drag.source.url )
                    drag.accept(false)
            }
            onDropped: {
                if(drag.source.url)
                {
                    textField.text = drag.source.localPath
                }
            }
        }
    }

    Button {
        id: openButton
        Layout.alignment: Qt.AlignTop

        Image {
            width: 16
            height: 16
            source: "qrc:/icon/open.png"
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit

        }
        onClicked: {
            /// Open the FileDialog at the specified location.
            var url = "";
            if( sofaData.properties.folderurl !== ""){
                url = "file://"+sofaData.properties.folderurl
            }else{
                url = SofaApplication.currentProject.rootDir
            }

            fileDialog.folder = url
            fileDialog.open()
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

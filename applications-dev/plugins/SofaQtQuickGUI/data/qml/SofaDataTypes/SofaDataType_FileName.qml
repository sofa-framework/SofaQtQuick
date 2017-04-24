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
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.0


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
    spacing : 10
    width: parent.width

    property var dataObject: null

    TextField {
        id: textField
        enabled: true
        width: root.width - openButton.width - root.spacing
        text: undefined !== dataObject.value ? dataObject.value.toString() : ""

        onAccepted: {
            /// Get the URL from the file chooser and convert it to a string.
            dataObject.value = textField.text ;
            dataObject.upload();
        }
    }

    Button {
        id: openButton
        Layout.alignment: Qt.AlignTop

        width: 16
        height: 16
        iconSource: "qrc:/icon/open.png"
        onClicked: {
            /// Open the FileDialog at the specified location.
            fileDialog.folder =  "file://"+dataObject.properties.folderurl
            fileDialog.open()
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: "file://"+dataObject.properties.folderurl
        onAccepted: {
            /// Get the URL from the file chooser and convert it to a string.
            dataObject.value = fileDialog.fileUrl.toString().replace("file://","") ;
            dataObject.upload();
            textField.text = dataObject.value.toString();
        }
    }

}

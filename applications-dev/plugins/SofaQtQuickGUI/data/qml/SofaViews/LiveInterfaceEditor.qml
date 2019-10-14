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
import SofaApplication 1.0
import QtQuick.Controls 1.4 as QQC1
import LiveFileMonitorSingleton 1.0
import SofaBasics 1.0

/*
    A Panel that monitor the SofaWidget directory to detect changes and automagically
    update its content to show the result for a nice live coding workflow.
*/
QQC1.SplitView {
    id: root
    orientation: Qt.Horizontal
    property var files : LiveFileMonitorSingleton.files

    property alias container: container
    property string fileContent
    onFileContentChanged: {
        var viewedcomponent=Qt.createQmlObject(fileContent, container)
    }

    Item {
        id: container
        implicitWidth: root.width / 2
        Layout.fillHeight: true
    }

    function readTextFile(fileUrl) {
        var xhr = new XMLHttpRequest;
        xhr.open("GET", fileUrl); // set Method and File
        xhr.onreadystatechange = function () {
            if(xhr.readyState === XMLHttpRequest.DONE){ // if request_status == DONE
                var response = xhr.responseText;

                root.fileContent = response
            }
        }
        xhr.send(); // begin the request
    }

    onFilesChanged: {
        container.children=""
        var fileUrl = "file://"+files+"?t="+Date.now()
        /// Reload the component using the non cached version (because of Data.now())
        readTextFile(fileUrl)
    }
}

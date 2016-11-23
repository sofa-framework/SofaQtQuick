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

import LiveFileMonitorSingleton 1.0

/*
    A Panel that monitor the SofaWidget directory to detect changes and automagically
    update its content to show the result for a nice live coding workflow.
*/
SplitView {
    id: root
    orientation: Qt.Horizontal
    property var filehaschanged : LiveFileMonitorSingleton.files

    property alias container: container

    Item {
        id: container
        implicitWidth: root.width / 2
        Layout.fillHeight: true
    }

    onFilehaschangedChanged: {
            container.children=""
            var viewedcomponent=Qt.createComponent("file://"+filehaschanged+"?t="+Date.now())
            if(viewedcomponent.status == Component.Ready)
            {
                viewedcomponent.createObject(container)
                console.log("Object updated !!!")
            }else{
                console.log("ZOB") ;
                var err = viewedcomponent.errorString();
                console.log("Error at " + err + "<br />")

            }
    }
}

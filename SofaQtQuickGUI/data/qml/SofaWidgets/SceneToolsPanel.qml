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

import LiveFileMonitorSingleton 1.0

ColumnLayout {
    id: root
    width: 300
    spacing: 0
    enabled: sofaScene ? sofaScene.ready : false

    property var sofaScene: SofaApplication.sofaScene

    readonly property Loader interfaceLoader: sofaScene ? sofaScene.interfaceLoader : null
    readonly property QtObject interfaceObject: interfaceLoader ? interfaceLoader.item : null
    readonly property Component interfaceToolPanel: interfaceObject ? interfaceObject.toolpanel : null

    property alias container: container
    property var files: LiveFileMonitorSingleton.files
    property var sourceqml: sofaScene ? sofaScene.sourceQML : null

    function updateContentFromSourceQML(urlname) {
        container.children=""
        /// Reload the component using the non cached version (because of Data.now())
        var viewedcomponent=Qt.createComponent(urlname)
        if(viewedcomponent.status == Component.Ready)
        {
            var obj=viewedcomponent.createObject()
            obj.toolpanel.createObject(container)
            //viewedcomponent.createObject(container)
            //container.children=[viewedcomponent.toolpanel]
            console.log("MMComponent updated !!!")
        }else{
            var err = viewedcomponent.errorString();
            console.log("Error in reloading "+ urlname + " " + err + "<br />")
        }
        return true
    }

    onSourceqmlChanged: {
        console.log("SOURCE QML CHANGED..."+sourceqml)
        console.log("REGISTERING: "+ LiveFileMonitorSingleton.addFile(sourceqml))
        updateContentFromSourceQML(sourceqml+"?t="+Date.now())
    }

    onFilesChanged: {
        var urlname="file://"+files+"?t="+Date.now()
        updateContentFromSourceQML(urlname)
    }

    Rectangle{
            id: header
            width: parent.width
            height: 20
            color: "darkgrey"
            Text{
                text : "Messages"
                font.pixelSize: 14
                font.bold: true
            }
    }

    Item {
        id: container
        implicitWidth: root.width / 2
        Layout.fillHeight: true
    }
}

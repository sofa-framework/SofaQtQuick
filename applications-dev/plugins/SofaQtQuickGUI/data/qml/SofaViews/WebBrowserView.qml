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
import QtGraphicalEffects 1.0
import QtWebEngine 1.5
import EditView 1.0
import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import SofaBasics 1.0
import SofaApplication 1.0
import SofaInteractors 1.0
import EditView 1.0
import SofaScene 1.0

Item{
    id: root
    property url url : "file:///home/dmarchal/projects/DEFROST/dev/spm/spm-recipes/plugins.html"

    ScrollView {
        id: scroll
        implicitWidth : parent.width
        implicitHeight : parent.height
        anchors.top : parent.top
        anchors.topMargin: 0
        z:0

        WebEngineView {
            z: -1
            id: webview
            url: root.url

            width: scroll.implicitWidth
            height: scroll.implicitHeight

            onUrlChanged:
            {
                console.log("LOADING CHANGED TO " + url)
                if(url.match("?installPlugin="))
                {
                    SofaApplication.runPythonScript("import subprocess\nsubprocess.Popen(['sofa-spm.py','install','SoftRobots'])")
                }
            }

            onLoadingChanged:
            {
                if (loadRequest.errorString)
                    console.error(loadRequest.errorString);
            }
        }
    }
}

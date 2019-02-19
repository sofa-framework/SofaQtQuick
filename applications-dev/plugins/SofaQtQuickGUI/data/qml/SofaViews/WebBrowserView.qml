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

//    EditView {
//        id: editView
//        width: 300
//        height: 300
//        z: 2
//        clip: true
//        backgroundColor: "#FF404040"
//        mirroredHorizontally: false
//        mirroredVertically: false
//        antialiasingSamples: 2
//        sofaScene: SofaApplication.sofaScene

//        property bool configurable: true
//        property bool transparentBackground: false
//        property int uiId: 0
//        property int previousUiId: uiId
//        onUiIdChanged: {
//            SofaApplication.uiSettings.replace(previousUiId, uiId);
//        }

//        Component.onCompleted: {
//            SofaApplication.addSofaViewer(root);

//            if(!SofaApplication.focusedSofaView)
//                forceActiveFocus();

//            if(root.sofaScene && root.sofaScene.ready)
//                recreateCamera();

//            loadCameraFromFile((Number(uiId)));
//        }

//        Component.onDestruction: {
//            SofaApplication.removeSofaViewer(root);
//        }
//    }

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
            url: "file:///home/dmarchal/projects/DEFROST/prez/GTAS2018/slides.html"

            width: scroll.implicitWidth
            height: scroll.implicitHeight

            onLoadingChanged: {
                if (loadRequest.errorString)
                    console.error(loadRequest.errorString);
            }
        }
    }
}

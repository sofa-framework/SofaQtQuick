// /*
// Copyright 2015, Anatoscope

// This file is part of sofaqtquick.

// sofaqtquick is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// sofaqtquick is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
// */

// import QtQuick 2.0
// import QtQuick.Controls 2.4
// import QtQuick.Layouts 1.1
// import QtQuick.Dialogs 1.2
// import QtGraphicalEffects 1.0
// import SofaBasics 1.0
// import AssetView 1.0
// import SofaScene 1.0

// AssetView {
//     readonly property string docstring :
//         "This view renders assets that can be placed in your scene"

//     id: root

//     property var rootNode: undefined
//     onRootNodeChanged: {
//         recreateCamera()
//     }

//     sofaScene: SofaScene {
//     }

//     clip: true
//     mirroredHorizontally: false
//     mirroredVertically: false
//     antialiasingSamples: 2
//     property int minimumValue: 2
//     property int maximumValue: 16

//     property bool configurable: true

//     implicitWidth: 800
//     implicitHeight: 600

//     Component.onCompleted: {
//         if(root.rootNode)
//             recreateCamera();
//     }

//     // camera
//     Component {
//         id: cameraComponent
//         SofaCamera {}
//     }

//     property bool keepCamera: false

//     function updateCameraFromIndex(index)
//     {
//         if(camera)
//         {
//             var listCameraInSofa = rootNode.componentsByType("BaseCamera");
//             camera.sofaComponent = (rootNode.componentsByType("BaseCamera").at(index));
//             if(listCameraInSofa.size() === 0)
//             {
//                 console.log("No Camera in the scene")
//             }
//             if(listCameraInSofa.size() < index)
//             {
//                 console.log("Error while trying to load camera number "+ index)
//             }
//             else
//             {
//                 camera.sofaComponent = listCameraInSofa.at(index);
//             }
//         }
//     }

//     function recreateCamera() {
//         if(camera && !keepCamera) {
//             camera.destroy();
//             camera = null;
//         }
//         if(!camera)
//         {
//             camera = cameraComponent.createObject(root, {orthographic: false} );
//             camera.bindCameraFromScene(root.SofaScene, 0);

//             //Todo fetch index from somewhere
//             var defaultIndex = 0;
//             updateCameraFromIndex(0);

//             if(!keepCamera)
//                 viewAll();
//         }
//     }

//     Connections {
//         target: root.sofaScene
//         onStatusChanged: {
//             if(root.sofaScene && SofaScene.Ready === root.sofaScene.status)
//                 root.recreateCamera();
//         }
//     }
// }
Item {
}

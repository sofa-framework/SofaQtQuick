import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Qt.labs.settings 1.0
import SofaBasics 1.0
import SofaManipulators 1.0
//import Scene 1.0

CollapsibleGroupBox {
    id: root

    title: "Scene Parameters"
    enabled: scene.ready

    Component {
        id: manipulator2DComponent

        Manipulator {
            id: manipulator

            property var scene
            property int index: -1
/*
            Manipulator2D_Translation {
                id: txy

                axis: "xy"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Translation {
                id: tx

                axis: "x"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Translation {
                id: ty

                axis: "y"

                onPositionChanged: manipulator.position = position;
            }
*/
            Manipulator2D_Rotation {
                id: rz

                onOrientationChanged: manipulator.orientation = orientation;
            }

            Component.onCompleted: getTransformation()

            onPositionChanged: setTransformation()
            onOrientationChanged: setTransformation()

            function getTransformation() {
                if(-1 === index)
                    return;

                var transformationArray = scene.pythonInteractor.call("moveController", "getTransformation", index);
                position = Qt.vector3d(transformationArray[0], transformationArray[1], transformationArray[2]);
                orientation = Qt.quaternion(transformationArray[6], transformationArray[3], transformationArray[4], transformationArray[5]);
            }

            function setTransformation() {
                if(-1 === index)
                    return;

                scene.pythonInteractor.call("moveController", "setTransformation", index, [position.x, position.y, position.z, orientation.x, orientation.y, orientation.z, orientation.scalar]);
            }

        }
    }

    Component {
        id: manipulator3DComponent

        Manipulator {
            id: manipulator

            property var scene
            property int index: -1
/*
            Manipulator3D_Translation {
                id: tx

                axis: "x"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: ty

                axis: "y"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: tz

                axis: "z"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: txy

                axis: "xy"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: txz

                axis: "xz"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: tyz

                axis: "yz"

                onPositionChanged: manipulator.position = position;
            }
*/
            Manipulator3D_Rotation {
                id: rx

                axis: "x"

                onOrientationChanged: manipulator.orientation = orientation;
            }

            Manipulator3D_Rotation {
                id: ry

                axis: "y"

                onOrientationChanged: manipulator.orientation = orientation;
            }

            Manipulator3D_Rotation {
                id: rz

                axis: "z"

                onOrientationChanged: manipulator.orientation = orientation;
            }

            Component.onCompleted: getTransformation()

            onPositionChanged: setTransformation()
            onOrientationChanged: setTransformation()

            function getTransformation() {
                var transformationArray = scene.pythonInteractor.call("moveController", "getTransformation", index);
                position = Qt.vector3d(transformationArray[0], transformationArray[1], transformationArray[2]);
                orientation = Qt.quaternion(transformationArray[6], transformationArray[3], transformationArray[4], transformationArray[5]);
            }

            function setTransformation() {
                scene.pythonInteractor.call("moveController", "setTransformation", index, [position.x, position.y, position.z, orientation.x, orientation.y, orientation.z, orientation.scalar]);
            }

            property var sceneConnection: Connections {
                target: scene ? scene : null
                onSelectedComponentsChanged: console.log("changed", scene.selectedComponent().name);
            }
        }
    }

    Component.onCompleted: {
        for(var i = 0; i < 5; ++i)
            scene.addManipulator(manipulator2DComponent.createObject(root, {scene: scene, index: i}));

        for(var i = 5; i < 10; ++i)
            scene.addManipulator(manipulator3DComponent.createObject(root, {scene: scene, index: i}));
    }
}

import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Qt.labs.settings 1.0
import SofaBasics 1.0
import SofaManipulators 1.0

CollapsibleGroupBox {
    id: root

    title: "Scene Parameters"
    enabled: scene.ready

    // define a view space manipulator component
    Component {
        id: manipulator2DComponent

        Manipulator {
            id: manipulator

            property var scene
            property int index: -1

            Manipulator2D_Translation {
                id: txy

                visible: manipulator.visible
                axis: "xy"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Translation {
                id: tx

                visible: manipulator.visible
                axis: "x"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Translation {
                id: ty

                visible: manipulator.visible
                axis: "y"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Rotation {
                id: rz

                visible: manipulator.visible

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

    // define a world space manipulator component
    Component {
        id: manipulator3DComponent

        Manipulator {
            id: manipulator

            property var scene
            property int index: -1

            Manipulator3D_Translation {
                id: tx

                visible: manipulator.visible
                axis: "x"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: ty

                visible: manipulator.visible
                axis: "y"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: tz

                visible: manipulator.visible
                axis: "z"

                onPositionChanged: manipulator.position = position;
            }

//            Manipulator3D_Translation {
//                id: txy

//                visible: manipulator.visible
//                axis: "xy"

//                onPositionChanged: manipulator.position = position;
//            }

//            Manipulator3D_Translation {
//                id: txz

//                visible: manipulator.visible
//                axis: "xz"

//                onPositionChanged: manipulator.position = position;
//            }

//            Manipulator3D_Translation {
//                id: tyz

//                visible: manipulator.visible
//                axis: "yz"

//                onPositionChanged: manipulator.position = position;
//            }

            Manipulator3D_Rotation {
                id: rx

                visible: manipulator.visible
                axis: "x"

                onOrientationChanged: manipulator.orientation = orientation;
            }

            Manipulator3D_Rotation {
                id: ry

                visible: manipulator.visible
                axis: "y"

                onOrientationChanged: manipulator.orientation = orientation;
            }

            Manipulator3D_Rotation {
                id: rz

                visible: manipulator.visible
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
        }
    }

    // retrieve the selected component in the scene
    property int currentIndex: -1
    Connections {
        target: scene
        onSelectedComponentsChanged: {
            var selectedComponent = scene.selectedComponent();
            if(selectedComponent) {
                var selectedComponentName = scene.selectedComponent().name;

                // deduce the component index from its name
                currentIndex = Number(selectedComponentName.slice(selectedComponentName.lastIndexOf("_") + 1));
            }
            else {
                // -1 means no component is selected
                currentIndex = -1;
            }
        }
    }

    Component.onCompleted: {
        // instanciate manipulators and show only if their index if the same as the selected component index
        // in this way, we only see manipulators of the selected component

        for(var i = 0; i < 5; ++i)
            scene.addManipulator(manipulator2DComponent.createObject(root, {scene: scene, index: i, visible: Qt.binding(function () {return this.index == root.currentIndex;})}));

        for(var i = 5; i < 10; ++i)
            scene.addManipulator(manipulator3DComponent.createObject(root, {scene: scene, index: i, visible: Qt.binding(function () {return this.index == root.currentIndex;})}));
    }
}

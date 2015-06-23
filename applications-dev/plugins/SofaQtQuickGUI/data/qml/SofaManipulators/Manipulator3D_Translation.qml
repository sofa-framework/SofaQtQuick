import QtQuick 2.0
import Manipulator3D_Translation 1.0

Manipulator3D_Translation {
    id: root

    property var baseVector: Qt.vector3d(0.0, 0.0, 0.0)

    function mousePressed(mouse, scene, viewer) {
        var pickedPosition = scene.pickingInteractor.pickedPosition();

        baseVector = pickedPosition.minus(root.position);
    }

    function mouseMoved(mouse, scene, viewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;
        var zAxis = -1 !== axis.indexOf("z") ? true : false;
        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

        if(0 !== axisNum) {
            // unproject from screen to world
            var nearPosition = viewer.mapToWorld(Qt.vector3d(mouse.x + 0.5, mouse.y + 0.5, 0.0));
            var z = viewer.camera.computeDepth(scene.pickingInteractor.pickedPosition());

            // project on a specific plane parallel to our view plane
            var position = viewer.camera.projectOnViewPlane(nearPosition, z);
            var direction = position.minus(root.position.plus(baseVector));

            position = root.position.plus(direction);
/*
            if(xAxis) {
                var axisVector = Qt.vector3d(1.0, 0.0, 0.0);
                console.log(axisVector.dotProduct(position));
                position = root.position.plus(axisVector.times(axisVector.dotProduct(position)));
            }

            if(yAxis) {
                var axisVector = Qt.vector3d(0.0, 1.0, 0.0);
                position = root.position.plus(axisVector.times(axisVector.dotProduct(position)));
            }

            if(zAxis) {
                var axisVector = Qt.vector3d(0.0, 0.0, 1.0);
                position = root.position.plus(axisVector.times(axisVector.dotProduct(position)));
            }
*/
            root.position = position;
            console.log("pos", root.position);
        }
    }
}

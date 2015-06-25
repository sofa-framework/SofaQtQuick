import QtQuick 2.0
import Manipulator2D_Translation 1.0

Manipulator2D_Translation {
    id: root

    property var baseVector: Qt.vector3d(0.0, 0.0, 0.0)

    function mousePressed(mouse, scene, viewer) {
        var pickedPosition = scene.pickingInteractor.pickedPosition();

        baseVector = pickedPosition.minus(root.position);
    }

    function mouseMoved(mouse, scene, viewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;

        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0);

        if(0 !== axisNum) {
            // unproject from screen to world
            var nearPosition = viewer.mapToWorld(Qt.point(mouse.x + 0.5, mouse.y + 0.5), 0.0);
            var z = viewer.camera.computeDepth(scene.pickingInteractor.pickedPosition());

            // project on a specific plane parallel to our view plane
            var position = viewer.camera.projectOnViewPlane(nearPosition, z);
            var direction = position.minus(root.position.plus(baseVector));

            // project on a specific axis aligned with the view space axis
            if(xAxis && yAxis) {  // or let it free on the view plane
                position = root.position.plus(direction);
            } else if(xAxis) {
                position = root.position.plus(viewer.camera.projectOnViewSpaceXAxis(direction));
            } else if(yAxis) {
                position = root.position.plus(viewer.camera.projectOnViewSpaceYAxis(direction));
            }

            root.position = position;
        }
    }
}

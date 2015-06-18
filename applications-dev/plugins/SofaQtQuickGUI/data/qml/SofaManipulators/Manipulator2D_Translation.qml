import QtQuick 2.0
import SofaBasics 1.0
import Manipulator2D_Translation 1.0

Manipulator2D_Translation {
    id: root

    property var baseVector: Qt.vector3d(0.0, 0.0, 0.0)

    function mousePressed(mouse, scene, viewer) {
        var pickedPosition = scene.pickingInteractor.pickedPosition();

        baseVector = pickedPosition.minus(root.position);
    }

    function mouseMoved(mouse, scene, viewer) {
        if(xAxis || yAxis) {
            // unproject from screen to world
            var nearPosition = viewer.mapToWorld(Qt.vector3d(mouse.x + 0.5, mouse.y + 0.5, 0.0));
            var z = viewer.camera.computeDepth(scene.pickingInteractor.pickedPosition());

            // project on a specific plane parallel to our view plane
            var position = viewer.camera.projectOnViewPlane(nearPosition, z);
            var direction = position.minus(root.position.plus(baseVector));

            // project on a specific axis aligned with the view space axis
            if(!xAxis) {
                position = root.position.plus(viewer.camera.projectOnViewSpaceYAxis(direction));
            } else if(!yAxis) {
                position = root.position.plus(viewer.camera.projectOnViewSpaceXAxis(direction));
            } else { // or let it free on the view plane
                position = root.position.plus(direction);
            }

            root.position = position;
        }
    }
}

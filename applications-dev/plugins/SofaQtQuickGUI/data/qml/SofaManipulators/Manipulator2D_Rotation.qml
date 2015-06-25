import QtQuick 2.0
import Manipulator2D_Rotation 1.0

Manipulator2D_Rotation {
    id: root

    property real baseAngle: 0.0

    function mousePressed(mouse, scene, viewer) {
        // unproject from screen to world
        var pickedPosition = scene.pickingInteractor.pickedPosition();
        var nearPosition = viewer.mapToWorld(Qt.point(mouse.x + 0.5, mouse.y + 0.5), 0.0);
        var z = viewer.camera.computeDepth(root.position);

        // project on a specific plane parallel to our view plane
        pickedPosition = viewer.camera.projectOnViewPlane(nearPosition, z);

        var direction = pickedPosition.minus(root.position).normalized();
        baseAngle = Math.acos(viewer.camera.up().dotProduct(direction));
        if(viewer.camera.right().dotProduct(direction) < 0.0)
            baseAngle = -baseAngle;
    }

    function mouseMoved(mouse, scene, viewer) {
        // unproject from screen to world
        var pickedPosition = scene.pickingInteractor.pickedPosition();
        var nearPosition = viewer.mapToWorld(Qt.point(mouse.x + 0.5, mouse.y + 0.5), 0.0);
        var z = viewer.camera.computeDepth(root.position);

        // project on a specific plane parallel to our view plane
        pickedPosition = viewer.camera.projectOnViewPlane(nearPosition, z);

        var direction = pickedPosition.minus(root.position).normalized();
        var angle = Math.acos(viewer.camera.up().dotProduct(direction));
        if(viewer.camera.right().dotProduct(direction) < 0.0)
            angle = -angle;

        setMark(baseAngle, angle);

        var deltaAngle = angle - baseAngle;
        var quatAngle = deltaAngle * 0.5;
        var quatAxis = viewer.camera.direction().times(Math.sin(quatAngle));
        var orientation = Qt.quaternion(Math.cos(quatAngle), quatAxis.x, quatAxis.y, quatAxis.z);
        root.orientation = orientation;
    }

    function mouseReleased(mouse, scene, viewer) {
        unsetMark();
    }
}

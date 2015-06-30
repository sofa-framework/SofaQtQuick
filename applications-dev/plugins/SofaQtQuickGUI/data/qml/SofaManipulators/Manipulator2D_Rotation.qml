import QtQuick 2.0
import Manipulator2D_Rotation 1.0

Manipulator2D_Rotation {
    id: root

    property real baseAngle: 0.0
    property var  baseOrientation

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

        baseOrientation = Qt.quaternion(root.orientation.scalar, root.orientation.x, root.orientation.y, root.orientation.z);
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

        var orientation = quaternionFromAxisAngle(viewer.camera.direction().normalized(), (angle - baseAngle) / Math.PI * 180.0);
        root.orientation = quaternionMultiply(orientation, baseOrientation);
    }

    function mouseReleased(mouse, scene, viewer) {
        unsetMark();
    }
}

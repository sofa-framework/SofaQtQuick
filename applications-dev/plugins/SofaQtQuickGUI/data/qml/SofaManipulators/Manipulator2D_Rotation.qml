import QtQuick 2.0
import Manipulator2D_Rotation 1.0

Manipulator2D_Rotation {
    id: root

    property real startAngle: 0.0
    property var  startOrientation

    function mousePressed(mouse, scene, viewer) {
        // project on a specific plane parallel to our view plane
        var z = viewer.computeDepth(root.position);
        var position = viewer.mapToWorld(Qt.point(mouse.x + 0.5, mouse.y + 0.5), z);

        var direction = position.minus(root.position).normalized();
        startAngle = Math.acos(viewer.camera.up().dotProduct(direction));
        if(viewer.camera.right().dotProduct(direction) < 0.0)
            startAngle = -startAngle;

        startOrientation = Qt.quaternion(root.orientation.scalar, root.orientation.x, root.orientation.y, root.orientation.z);
    }

    function mouseMoved(mouse, scene, viewer) {
        // project on a specific plane parallel to our view plane
        var z = viewer.computeDepth(root.position);
        var position = viewer.mapToWorld(Qt.point(mouse.x + 0.5, mouse.y + 0.5), z);

        var direction = position.minus(root.position).normalized();
        var angle = Math.acos(viewer.camera.up().dotProduct(direction));
        if(viewer.camera.right().dotProduct(direction) < 0.0)
            angle = -angle;

        setMark(startAngle, angle);

        var orientation = quaternionFromAxisAngle(viewer.camera.direction().normalized(), (angle - startAngle) / Math.PI * 180.0);
        root.orientation = quaternionMultiply(orientation, startOrientation);
    }

    function mouseReleased(mouse, scene, viewer) {
        unsetMark();
    }
}

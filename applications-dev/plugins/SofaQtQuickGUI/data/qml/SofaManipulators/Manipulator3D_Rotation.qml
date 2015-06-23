import QtQuick 2.0
import Manipulator3D_Rotation 1.0

Manipulator3D_Rotation {
    id: root

    property real baseAngle: 0.0

    function mousePressed(mouse, scene, viewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;
        var zAxis = -1 !== axis.indexOf("z") ? true : false;
        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

        if(1 === axisNum) {
            // unproject from screen to world
            var pickedPosition = scene.pickingInteractor.pickedPosition();
            var nearPosition = viewer.mapToWorld(Qt.vector3d(mouse.x + 0.5, mouse.y + 0.5, 0.0));
            var z = viewer.camera.computeDepth(root.position);

            // project on a specific plane parallel to our view plane
            pickedPosition = viewer.camera.projectOnViewPlane(nearPosition, z);
            var direction = pickedPosition.minus(root.position).normalized();

            var up;
            var right;
            var front;

            if(xAxis) {
                up = Qt.vector3d(0.0, 1.0, 0.0);
                right = Qt.vector3d(0.0, 0.0, -1.0);
                front = Qt.vector3d(-1.0, 0.0, 0.0);
            } else if(yAxis) {
                up = Qt.vector3d(0.0, 0.0, -1.0);
                right = Qt.vector3d(1.0, 0.0, 0.0);
                front = Qt.vector3d(0.0, -1.0, 0.0);
            } else if(zAxis) {
                up = Qt.vector3d(0.0, 1.0, 0.0);
                right = Qt.vector3d(1.0, 0.0, 0.0);
                front = Qt.vector3d(0.0, 0.0, -1.0);
            }

            //if(right.dotProduct(viewer.camera.right()) < 0.0 || up.dotProduct(viewer.camera.up()) < 0.0)
            //    front = Qt.vector3d(0.0, 0.0, 0.0).minus(front);

            baseAngle = Math.acos(up.dotProduct(direction));
            if(right.dotProduct(direction) < 0.0)
                baseAngle = -baseAngle;
        }
    }

    function mouseMoved(mouse, scene, viewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;
        var zAxis = -1 !== axis.indexOf("z") ? true : false;
        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

        if(1 === axisNum) {
            // unproject from screen to world
            var pickedPosition = scene.pickingInteractor.pickedPosition();
            var nearPosition = viewer.mapToWorld(Qt.vector3d(mouse.x + 0.5, mouse.y + 0.5, 0.0));
            var z = viewer.camera.computeDepth(root.position);

            // TODO: project on the correct plan not the view one, i.e XY for Z XZ for Y and YZ for X

            // project on a specific plane parallel to our view plane
            pickedPosition = viewer.camera.projectOnViewPlane(nearPosition, z);
            var direction = pickedPosition.minus(root.position).normalized();

            var up;
            var right;
            var front;

            if(xAxis) {
                up = Qt.vector3d(0.0, 1.0, 0.0);
                right = Qt.vector3d(0.0, 0.0, -1.0);
                front = Qt.vector3d(-1.0, 0.0, 0.0);
            } else if(yAxis) {
                up = Qt.vector3d(0.0, 0.0, -1.0);
                right = Qt.vector3d(1.0, 0.0, 0.0);
                front = Qt.vector3d(0.0, -1.0, 0.0);
            } else if(zAxis) {
                up = Qt.vector3d(0.0, 1.0, 0.0);
                right = Qt.vector3d(1.0, 0.0, 0.0);
                front = Qt.vector3d(0.0, 0.0, -1.0);
            }

            //if(right.dotProduct(viewer.camera.right()) < 0.0 || up.dotProduct(viewer.camera.up()) < 0.0)
            //    front = Qt.vector3d(0.0, 0.0, 0.0).minus(front);

            var angle = Math.acos(up.dotProduct(direction));
            if(right.dotProduct(direction) < 0.0)
                angle = -angle;

            setMark(baseAngle, angle);

            var deltaAngle = angle - baseAngle;
            var quatAngle = deltaAngle * 0.5;
            var quatAxis = front.times(Math.sin(quatAngle));
            var orientation = Qt.quaternion(Math.cos(quatAngle), quatAxis.x, quatAxis.y, quatAxis.z);
            root.orientation = orientation;
        }
    }

    function mouseReleased(mouse, scene, viewer) {
        unsetMark();
    }
}

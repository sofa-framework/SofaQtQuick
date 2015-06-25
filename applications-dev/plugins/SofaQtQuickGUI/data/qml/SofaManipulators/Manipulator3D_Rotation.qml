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
            var normalVector = Qt.vector3d(xAxis ? 1.0 : 0.0, yAxis ? 1.0 : 0.0, zAxis ? 1.0 : 0.0);
            var direction = viewer.projectOnPlane(Qt.point(mouse.x + 0.5, mouse.y + 0.5), root.position, normalVector).minus(root.position).normalized();

            var right = Qt.vector3d(yAxis || zAxis ?  1.0 : 0.0,                        0.0, xAxis ? -1.0 : 0.0);
            var up    = Qt.vector3d(                        0.0, xAxis || zAxis ? 1.0 : 0.0, yAxis ? -1.0 : 0.0);
            var front = Qt.vector3d(         xAxis ? -1.0 : 0.0,          yAxis ? 1.0 : 0.0, zAxis ?  1.0 : 0.0);

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
            var normalVector = Qt.vector3d(xAxis ? 1.0 : 0.0, yAxis ? 1.0 : 0.0, zAxis ? 1.0 : 0.0);
            var direction = viewer.projectOnPlane(Qt.point(mouse.x + 0.5, mouse.y + 0.5), root.position, normalVector).minus(root.position).normalized();

            var right = Qt.vector3d(yAxis || zAxis ?  1.0 : 0.0,                        0.0, xAxis ? -1.0 : 0.0);
            var up    = Qt.vector3d(                        0.0, xAxis || zAxis ? 1.0 : 0.0, yAxis ? -1.0 : 0.0);
            var front = Qt.vector3d(         xAxis ? -1.0 : 0.0,          yAxis ? 1.0 : 0.0, zAxis ?  1.0 : 0.0);

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

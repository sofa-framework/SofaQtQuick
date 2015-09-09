import QtQuick 2.0
import Manipulator3D_Translation 1.0

Manipulator3D_Translation {
    id: root

    property var startVector: Qt.vector3d(0.0, 0.0, 0.0)

    function mousePressed(mouse, viewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;
        var zAxis = -1 !== axis.indexOf("z") ? true : false;
        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

        if(2 === axisNum) { // project on a specific plane
            var normalVector = Qt.vector3d(!xAxis ? 1.0 : 0.0, !yAxis ? 1.0 : 0.0, !zAxis ? 1.0 : 0.0);
            startVector = viewer.projectOnPlane(Qt.point(mouse.x + 0.5, mouse.y + 0.5), root.position, normalVector).minus(root.position);
        } else if(1 === axisNum) {
            var axisVector = Qt.vector3d(xAxis ? 1.0 : 0.0, yAxis ? 1.0 : 0.0, zAxis ? 1.0 : 0.0);
            startVector = viewer.projectOnLine(Qt.point(mouse.x + 0.5, mouse.y + 0.5), root.position, axisVector).minus(root.position);
        }
    }

    function mouseMoved(mouse, viewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;
        var zAxis = -1 !== axis.indexOf("z") ? true : false;
        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

        if(2 === axisNum) { // project on a specific plane
            var normalVector = Qt.vector3d(!xAxis ? 1.0 : 0.0, !yAxis ? 1.0 : 0.0, !zAxis ? 1.0 : 0.0);
            var direction = viewer.projectOnPlane(Qt.point(mouse.x + 0.5, mouse.y + 0.5), root.position, normalVector).minus(root.position);
            var position = root.position.plus(direction).minus(startVector);

            root.position = position;
        } else if(1 === axisNum) { // project on a specific axis
            var axisVector = Qt.vector3d(xAxis ? 1.0 : 0.0, yAxis ? 1.0 : 0.0, zAxis ? 1.0 : 0.0);
            var direction = viewer.projectOnLine(Qt.point(mouse.x + 0.5, mouse.y + 0.5), root.position, axisVector).minus(root.position);
            var position = root.position.plus(direction).minus(startVector);

            root.position = position;
        }
    }
}

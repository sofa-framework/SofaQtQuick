/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Manipulator3D_Rotation 1.0

Manipulator3D_Rotation {
    id: root

    property real startAngle: 0.0
    property var  startOrientation

    function mousePressed(mouse, viewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;
        var zAxis = -1 !== axis.indexOf("z") ? true : false;
        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

        if(1 === axisNum) {
            var normalVector = Qt.vector3d(xAxis ? 1.0 : 0.0, yAxis ? 1.0 : 0.0, zAxis ? 1.0 : 0.0);
            var direction = viewer.projectOnPlane(Qt.point(mouse.x + 0.5, mouse.y + 0.5), root.position, normalVector).minus(root.position).normalized();

            var right = Qt.vector3d(yAxis || zAxis ?  1.0 : 0.0,                        0.0, xAxis ? -1.0 : 0.0);
            var up    = Qt.vector3d(                        0.0, xAxis || zAxis ? 1.0 : 0.0, yAxis ? -1.0 : 0.0);
            var front = Qt.vector3d(         xAxis ? -1.0 : 0.0,          yAxis ?-1.0 : 0.0, zAxis ? -1.0 : 0.0);

            startAngle = Math.acos(up.dotProduct(direction));
            if(right.dotProduct(direction) < 0.0)
                startAngle = -startAngle;

            startOrientation = Qt.quaternion(root.orientation.scalar, root.orientation.x, root.orientation.y, root.orientation.z);
        }
    }

    function mouseMoved(mouse, viewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;
        var zAxis = -1 !== axis.indexOf("z") ? true : false;
        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

        if(1 === axisNum) {
            var normalVector = Qt.vector3d(xAxis ? 1.0 : 0.0, yAxis ? 1.0 : 0.0, zAxis ? 1.0 : 0.0);
            var direction = viewer.projectOnPlane(Qt.point(mouse.x + 0.5, mouse.y + 0.5), root.position, normalVector).minus(root.position).normalized();

            var right = Qt.vector3d(yAxis || zAxis ?  1.0 : 0.0,                        0.0, xAxis ? -1.0 : 0.0);
            var up    = Qt.vector3d(                        0.0, xAxis || zAxis ? 1.0 : 0.0, yAxis ? -1.0 : 0.0);
            var front = Qt.vector3d(         xAxis ? -1.0 : 0.0,          yAxis ?-1.0 : 0.0, zAxis ? -1.0 : 0.0);

            var angle = Math.acos(up.dotProduct(direction));
            if(right.dotProduct(direction) < 0.0)
                angle = -angle;

            setMark(startAngle, angle);

            var orientation = quaternionFromAxisAngle(front, (angle - startAngle) / Math.PI * 180.0);
            root.orientation = quaternionMultiply(orientation, startOrientation);
        }
    }

    function mouseReleased(mouse, viewer) {
        unsetMark();
    }
}

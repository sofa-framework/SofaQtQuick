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
import Manipulator3D_Translation 1.0

Manipulator3D_Translation {
    id: root

    property var startVector: Qt.vector3d(0.0, 0.0, 0.0)

    function mousePressed(mouse, sofaViewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;
        var zAxis = -1 !== axis.indexOf("z") ? true : false;
        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

        if(2 === axisNum) { // project on a specific plane
            var normalVector = Qt.vector3d(!xAxis ? 1.0 : 0.0, !yAxis ? 1.0 : 0.0, !zAxis ? 1.0 : 0.0);
            startVector = sofaViewer.projectOnPlane(Qt.point(mouse.x, mouse.y), root.position, normalVector).minus(root.position);
        } else if(1 === axisNum) {
            var axisVector = Qt.vector3d(xAxis ? 1.0 : 0.0, yAxis ? 1.0 : 0.0, zAxis ? 1.0 : 0.0);
            startVector = sofaViewer.projectOnLine(Qt.point(mouse.x, mouse.y), root.position, axisVector).minus(root.position);
        }
    }

    function mouseMoved(mouse, sofaViewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;
        var zAxis = -1 !== axis.indexOf("z") ? true : false;
        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

        if(2 === axisNum) { // project on a specific plane
            var normalVector = Qt.vector3d(!xAxis ? 1.0 : 0.0, !yAxis ? 1.0 : 0.0, !zAxis ? 1.0 : 0.0);
            var direction = sofaViewer.projectOnPlane(Qt.point(mouse.x, mouse.y), root.position, normalVector).minus(root.position);
            var position = root.position.plus(direction).minus(startVector);

            root.position = position;
        } else if(1 === axisNum) { // project on a specific axis
            var axisVector = Qt.vector3d(xAxis ? 1.0 : 0.0, yAxis ? 1.0 : 0.0, zAxis ? 1.0 : 0.0);
            var direction = sofaViewer.projectOnLine(Qt.point(mouse.x, mouse.y), root.position, axisVector).minus(root.position);
            var position = root.position.plus(direction).minus(startVector);

            root.position = position;
        }
    }
}

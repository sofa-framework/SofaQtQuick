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
import Manipulator2D_Translation 1.0

Manipulator2D_Translation {
    id: root

    property var startVector: Qt.vector3d(0.0, 0.0, 0.0)
    name: "Manipulator2D_Translation"

    function mousePressed(mouse, sofaViewer) {
        var z = sofaViewer.computeDepth(root.position);
        var startPosition = sofaViewer.mapToWorld(Qt.point(mouse.x, mouse.y), z);

        startVector = startPosition.minus(root.position);
    }

    function mouseMoved(mouse, sofaViewer) {
        var xAxis = -1 !== axis.indexOf("x") ? true : false;
        var yAxis = -1 !== axis.indexOf("y") ? true : false;

        var axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0);

        if(0 !== axisNum) {
            // project on a specific plane parallel to our view plane
            var z = sofaViewer.computeDepth(root.position);
            var position = sofaViewer.mapToWorld(Qt.point(mouse.x, mouse.y), z);
            var direction = position.minus(root.position.plus(startVector));

            // project on a specific axis aligned with the view space axis
            if(xAxis && yAxis) {  // or let it free on the view plane
                position = root.position.plus(direction);
            } else if(xAxis) {
                position = root.position.plus(sofaViewer.camera.projectOnViewSpaceXAxis(direction));
            } else if(yAxis) {
                position = root.position.plus(sofaViewer.camera.projectOnViewSpaceYAxis(direction));
            }

            root.position = position;
        }
    }
}

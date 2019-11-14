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
import Manipulator3D_InPlaneTranslation 1.0

Manipulator3D_InPlaneTranslation {
    id: root

    property string name: "Manipulator3D_InPlaneTranslation"

    property var startVector: Qt.vector3d(0.0, 0.0, 0.0)

    function mousePressed(mouse, sofaViewer) {
        startVector = sofaViewer.projectOnPlane(Qt.point(mouse.x, mouse.y), root.position, sofaViewer.camera.direction());
    }

    function mouseMoved(mouse, sofaViewer) {
        var newPosition = sofaViewer.projectOnPlane(Qt.point(mouse.x, mouse.y), root.position, sofaViewer.camera.direction());
        root.position = newPosition;
    }
}

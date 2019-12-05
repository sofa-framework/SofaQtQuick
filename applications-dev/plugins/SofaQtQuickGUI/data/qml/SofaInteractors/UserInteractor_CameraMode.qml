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
import SofaBasics 1.0
import SofaApplication 1.0

UserInteractor {
    id: root

    property int previousX: -1
    property int previousY: -1

    property real moveSpeed: 0.00133
    property real turnSpeed: 20.0
    property real zoomSpeed: 1.0

    function moveCamera_init() {

        addMouseDoubleClickedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            var position = sofaViewer.projectOnGeometry(Qt.point(mouse.x, mouse.y));
            if(1.0 === position.w) {
                sofaViewer.camera.target = position.toVector3d();
                sofaViewer.crosshairGizmo.pop();
            }
        });

        addMousePressedMapping(Qt.RightButton, function(mouse, sofaViewer) {
            previousX = mouse.x;
            previousY = mouse.y;

            sofaViewer.crosshairGizmo.show();
            SofaApplication.overrideCursorShape = Qt.ClosedHandCursor;

            setMouseMovedMapping(function(mouse, sofaViewer) {
                if(!sofaViewer.camera)
                    return;

                var angleAroundX = 0.0;
                var angleAroundY = 0.0;
                var angleAroundZ = 0.0;

                if(Qt.ControlModifier & mouse.modifiers) {
                    angleAroundZ = (previousX - mouse.x) / 180.0 * Math.PI * turnSpeed;
                } else {
                    angleAroundX = (previousY - mouse.y) / 180.0 * Math.PI * turnSpeed;
                    angleAroundY = (previousX - mouse.x) / 180.0 * Math.PI * turnSpeed;
                }

                sofaViewer.camera.turn(angleAroundX, angleAroundY, angleAroundZ);

                previousX = mouse.x;
                previousY = mouse.y;
            });
        });

        addMouseReleasedMapping(Qt.RightButton, function(mouse, sofaViewer) {
            setMouseMovedMapping(null);

            SofaApplication.overrideCursorShape = 0;
            sofaViewer.crosshairGizmo.hide();
        });

        addMouseDoubleClickedMapping(Qt.RightButton, function(mouse, sofaViewer) {
            sofaViewer.camera.alignCameraAxis();
        });

//        addMousePressedMapping(Qt.MiddleButton, function(mouse, sofaViewer) {
//            previousX = mouse.x;
//            previousY = mouse.y;

//            sofaViewer.crosshairGizmo.show();
//            SofaApplication.overrideCursorShape = Qt.ClosedHandCursor;

//            setMouseMovedMapping(function(mouse) {
//                if(!sofaViewer.camera)
//                    return;

//                var screenToScene = sofaViewer.camera.target.minus(sofaViewer.camera.eye()).length();

//                var moveX = (mouse.x - previousX) * screenToScene * moveSpeed;
//                var moveY = (mouse.y - previousY) * screenToScene * moveSpeed;
//                sofaViewer.camera.move(-moveX, moveY, 0.0);

//                previousX = mouse.x;
//                previousY = mouse.y;
//            });
//        });

//        addMouseReleasedMapping(Qt.MiddleButton, function(mouse, sofaViewer) {
//            setMouseMovedMapping(null);

//            SofaApplication.overrideCursorShape = 0;
//            sofaViewer.crosshairGizmo.hide();
//        });

        setMouseWheelMapping(function(wheel, sofaViewer) {
            
            if(!sofaViewer.camera)
                return;

            if(0 === wheel.angleDelta.y)
                return;

            var boundary = 2.0;
            var factor = Math.max(-boundary, Math.min(wheel.angleDelta.y / 120.0, boundary)) / boundary;
            if(factor < 0.0) {
                factor = 1.0 + 0.5 * factor;
                factor /= zoomSpeed;
            }
            else {
                factor = 1.0 + factor;
                factor *= zoomSpeed;
            }

            sofaViewer.camera.zoom(factor);

            wheel.accepted = true;
        });
        
    }

    function init() {
        moveCamera_init();

        addMousePressedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            var selectable = sofaViewer.pickObject(Qt.point(mouse.x, mouse.y));
            if(selectable && selectable.sofaComponent) {
                console.error("SelectedCompoennt: " + SofaApplication.selectedComponent ? SofaApplication.selectedComponent.getName() : "null" + " -> " + selectable.sofaComponent)
                SofaApplication.selectedComponent = selectable.sofaComponent;
            }

            if(selectable && selectable.manipulator) {
                var manipulator = SofaApplication.selectedManipulator = selectable.manipulator
                if(manipulator.mousePressed)
                    manipulator.mousePressed(mouse, sofaViewer);

                if(manipulator.manipulate)
                    setMouseMovedMapping(manipulator.manipulate)
            }
        });

        addMouseReleasedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            var manipulator = SofaApplication.selectedManipulator

            if (manipulator)
                manipulator.index = -1;

            setMouseMovedMapping(null);
        });

    }

}

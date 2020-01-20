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
import QtQuick.Controls 2.0
import SofaBasics 1.0
import SofaApplication 1.0


UserInteractor {
    id: root

    property int previousX: -1
    property int previousY: -1

    property real moveSpeed: 0.00133
    property real turnSpeed: 20.0
    property real zoomSpeed: 1.0
    property var label

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

        addMousePressedMapping(Qt.MiddleButton, function(mouse, sofaViewer) {
            previousX = mouse.x;
            previousY = mouse.y;

            sofaViewer.crosshairGizmo.show();
            SofaApplication.overrideCursorShape = Qt.ClosedHandCursor;

            setMouseMovedMapping(function(mouse) {
                if(!sofaViewer.camera)
                    return;

                var screenToScene = sofaViewer.camera.target.minus(sofaViewer.camera.eye()).length();

                var moveX = (mouse.x - previousX) * screenToScene * moveSpeed;
                var moveY = (mouse.y - previousY) * screenToScene * moveSpeed;
                sofaViewer.camera.move(-moveX, moveY, 0.0);

                previousX = mouse.x;
                previousY = mouse.y;
            });
        });

        addMouseReleasedMapping(Qt.MiddleButton, function(mouse, sofaViewer) {
            setMouseMovedMapping(null);

            SofaApplication.overrideCursorShape = 0;
            sofaViewer.crosshairGizmo.hide();
        });

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
        SofaApplication.selectedManipulator = null
        addMousePressedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            console.log("Mouse pressed")
            var particle = sofaViewer.pickParticle(Qt.point(mouse.x, mouse.y));
            if(particle) {
                console.log("particle found")
                if (particle.sofaComponent) {
                    console.error("SelectedComponent: " + particle.sofaComponent)
                    SofaApplication.selectedComponent = particle.sofaComponent;
                }
                var manipulator = SofaApplication.selectedManipulator
                if(manipulator) {
                    manipulator.particleIndex = particle.particleIndex
                    manipulator.mousePressed(Qt.point(mouse.x, mouse.y), sofaViewer);
                    if (manipulator.displayText !== "")
                    {
                        label = Qt.createQmlObject('import QtQuick.Controls 2.0;
                                                    Label {
                                                       id: label
                                                       text: "plop"
                                                       color: "white"
                                                       onTextChanged: {
                                                          if (text === "")
                                                             label.destroy();
                                                       }
                                                    }', sofaViewer, 'label');
                        label.text = Qt.binding(function(){ return manipulator.displayText });
                        label.x = Qt.binding(function(){ return mouse.x + 15});
                        label.y = Qt.binding(function(){ return mouse.y - 15});
                        label.visible = false;
                    }
                    setMouseMovedMapping(function(m, sofaViewer){
                        console.log("AAA")
                        console.log(m.button)
                        console.log(m.buttons)
                        if (manipulator.displayText !== "") {
                            label.x = m.x + 15
                            label.y = m.y - 15
                            label.visible = true;
                        }
                        manipulator.mouseMoved(m, sofaViewer)
                    })
                }
            }
            var manip = sofaViewer.pickObject(Qt.point(mouse.x, mouse.y));
            if (manip.sofaComponent)
                console.log("object found")
            if (manip && manip.manipulator) {
                console.log("it's a manipulator")
                if (manip.sofaComponent) {
                    SofaApplication.selectedComponent = manip.sofaComponent;
                }

                manip = manip.manipulator
                if(manip.mousePressed)
                    manip.mousePressed(Qt.point(mouse.x, mouse.y), sofaViewer);

                if (manip.displayText !== "")
                {
                    label = Qt.createQmlObject('import QtQuick.Controls 2.0;
                                                Label {
                                                   id: label
                                                   text: "plop"
                                                   color: "white"
                                                   onTextChanged: {
                                                       if (text === "")
                                                           label.destroy();
                                                   }
                                                }', sofaViewer, 'label');
                    label.text = Qt.binding(function(){ return manip.displayText });
                    label.x = Qt.binding(function(){ return mouse.x + 15});
                    label.y = Qt.binding(function(){ return mouse.y - 15});
                    label.visible = false
                }
                setMouseMovedMapping(function(m, sofaViewer) {
                    console.log("BBB")
                    console.log(m.button)
                    console.log(m.buttons)
                    if (manip.displayText !== "") {
                        label.x = m.x + 15
                        label.y = m.y - 15
                        label.visible = true
                    }
                    manip.mouseMoved(m, sofaViewer)
                })
            }
        });

        addMouseReleasedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            var manipulator = SofaApplication.selectedManipulator

            if (manipulator) {
                manipulator.mouseReleased(Qt.point(mouse.x, mouse.y), sofaViewer);
                manipulator.index = -1;
            }
            setMouseMovedMapping(null);
            print("mouseReleased")

        });

    }

}
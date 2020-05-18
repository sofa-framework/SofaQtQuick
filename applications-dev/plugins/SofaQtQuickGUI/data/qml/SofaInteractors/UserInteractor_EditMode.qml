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
import SofaDataContainerListModel 1.0

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
            {
                print("no camera")
                return;
            }

            if(0 === wheel.angleDelta.y)
            {
                print("wheel angle delta y is 0")
                return;
            }

            var boundary = 2.0;
            var factor = Math.max(-boundary, Math.min(wheel.angleDelta.y / 120.0, boundary)) / boundary;
            print(factor)
            if(factor < 0.0) {
                factor = 1.0 + 0.5 * factor;
                factor /= zoomSpeed;
            }
            else {
                factor = 1.0 + factor;
                factor *= zoomSpeed;
            }

            sofaViewer.camera.zoom(factor);
            print("Zoom factor is: " + factor)

            wheel.accepted = true;
        });
        
    }


    property var currentManipulator: null
    property var model: SofaDataContainerListModel {
        asGridViewModel: false
    }
    property var ctrlPressed: false

    function init() {
        moveCamera_init();

        addKeyPressedMapping(Qt.Key_Control, function(key, sofaViewer) {
            ctrlPressed = true
        })
        addKeyReleasedMapping(Qt.Key_Control, function(key, sofaViewer) {
            ctrlPressed = false
        })

        addMousePressedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            if (sofaViewer.getManipulator("Snapping_Manipulator").enabled === true) {
                var selectable = sofaViewer.pickObject(Qt.point(mouse.x, mouse.y));
                if(selectable && selectable.sofaComponent) {
                    model.sofaData = SofaApplication.selectedComponent.findData("position")
                    var p = selectable.position
                    model.insertRow([p.x, p.y, p.z])
                    SofaApplication.selectedComponent.init()
                    SofaApplication.selectedComponent.reinit()
                    SofaApplication.selectedComponent.findData("showObject").value = 1
                    return
                }
            }
            var particle = sofaViewer.pickParticle(Qt.point(mouse.x, mouse.y));
            if(particle) {
                console.log("particle found")
                if (particle.sofaComponent) {
                    console.error("SelectedComponent: " + particle.sofaComponent)
                    SofaApplication.selectedComponent = particle.sofaComponent;
                }
                currentManipulator = null
                for (var i = 0 ; i < sofaViewer.manipulators.length ; ++i) {
                    sofaViewer.manipulators[i].isEditMode = true
                    sofaViewer.manipulators[i].particleIndex = particle.particleIndex
                    if (sofaViewer.manipulators[i].enabled) {
                        currentManipulator = sofaViewer.manipulators[i];
                    }
                }
                if(currentManipulator) {
                    console.log("selected manipulator: " + currentManipulator.name)
//                    currentManipulator = particle.manipulator
//                    SofaApplication.getManipulators()[i].particleIndex = particle.particleIndex
                    currentManipulator.mousePressed(Qt.point(mouse.x, mouse.y), sofaViewer);
                    if (currentManipulator.displayText !== "")
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
                        label.text = Qt.binding(function(){ return currentManipulator ? currentManipulator.displayText : "" });
                        label.x = Qt.binding(function(){ return mouse.x + 15});
                        label.y = Qt.binding(function(){ return mouse.y - 15});
                        label.visible = false;
                    }
                    setMouseMovedMapping(function(m, sofaViewer){
                        if (currentManipulator.displayText !== "") {
                            label.x = m.x + 15
                            label.y = m.y - 15
                        }
                        currentManipulator.mouseMoved(m, sofaViewer)
                    })
                }
//                return
            }
//            currentManipulator = null
            var object = sofaViewer.pickObject(Qt.point(mouse.x, mouse.y));
            if (object && object.sofaComponent)
                console.log("object found")
            if (object && object.manipulator) {
                console.log("it's a manipulator")
                if (object.sofaComponent) {
                    SofaApplication.selectedComponent = object.sofaComponent;
                }

                currentManipulator = object.manipulator
                if(currentManipulator.mousePressed)
                    currentManipulator.mousePressed(Qt.point(mouse.x, mouse.y), sofaViewer);

                if (currentManipulator.displayText !== "")
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
                    label.text = Qt.binding(function(){ return currentManipulator ? currentManipulator.displayText : "" });
                    label.x = Qt.binding(function(){ return mouse.x + 15});
                    label.y = Qt.binding(function(){ return mouse.y - 15});
                }
                setMouseMovedMapping(function(m, sofaViewer) {
                    if (currentManipulator.displayText !== "") {
                        label.x = m.x + 15
                        label.y = m.y - 15
                    }
                    currentManipulator.mouseMoved(m, sofaViewer)
                })
            }
        });

        addMouseReleasedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            if (currentManipulator) {
                currentManipulator.mouseReleased(Qt.point(mouse.x, mouse.y), sofaViewer);
                currentManipulator.index = -1;
                currentManipulator = null
            }
            setMouseMovedMapping(null);
        });

    }

}

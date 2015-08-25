import QtQuick 2.0
import SofaBasics 1.0
import SceneComponent 1.0
import "qrc:/SofaCommon/SofaToolsScript.js" as SofaToolsScript

UserInteractor_MoveCamera {
    id: root

    property var selectedManipulator: null
    property var selectedComponent: null

    function init() {
        moveCamera_init();
        addMousePressedMapping(Qt.LeftButton, function(mouse) {
            selectedManipulator = scene.selectedManipulator();
            selectedComponent = scene.selectedComponent();

            var selectable = viewer.pickObject(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
            if(selectable) {
                if(selectable.manipulator) {
                    selectedManipulator = selectable.manipulator;
                } else if(selectable.sceneComponent) {
                    selectedComponent = selectable.sceneComponent;
                }
            }

            if(selectedManipulator) {
                scene.setSelectedManipulator(selectedManipulator);

                if(selectedManipulator.mousePressed)
                    selectedManipulator.mousePressed(mouse, scene, viewer);

                if(selectedManipulator.mouseMoved)
                    setMouseMoveMapping(selectedManipulator.mouseMoved);

            } else if(selectedComponent) {
                if(!scene.areSameComponent(scene.selectedComponent(), selectedComponent)) {
                    scene.setSelectedComponent(selectedComponent);
                } else {
                    var sceneComponentParticle = viewer.pickParticle(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
                    if(sceneComponentParticle) {
                        scene.particleInteractor.start(sceneComponentParticle.sceneComponent, sceneComponentParticle.particleIndex);

                        setMouseMoveMapping(function(mouse) {
                            var z = viewer.computeDepth(scene.particleInteractor.particlePosition());
                            var position = viewer.mapToWorld(Qt.point(mouse.x + 0.5, mouse.y + 0.5), z);
                            scene.particleInteractor.update(position);
                        });
                    }
                }
            }
        });

        addMouseReleasedMapping(Qt.LeftButton, function(mouse) {
            if(scene.particleInteractor)
                scene.particleInteractor.release();

            if(selectedManipulator && selectedManipulator.mouseReleased)
                selectedManipulator.mouseReleased(mouse, scene, viewer);

            scene.clearSelectedManipulators();

            setMouseMoveMapping(null);
        });

    }
}

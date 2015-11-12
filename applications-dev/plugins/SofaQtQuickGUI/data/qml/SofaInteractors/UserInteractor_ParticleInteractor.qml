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

        addMousePressedMapping(Qt.LeftButton, function(mouse, viewer) {
            selectedManipulator = scene.selectedManipulator;
            selectedComponent = scene.selectedComponent;

            var sceneComponentParticle = viewer.pickParticle(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
            if(sceneComponentParticle) {
                scene.particleInteractor.start(sceneComponentParticle.sceneComponent, sceneComponentParticle.particleIndex);

                setMouseMovedMapping(function(mouse, viewer) {
                    var z = viewer.computeDepth(scene.particleInteractor.particlePosition());
                    var position = viewer.mapToWorld(Qt.point(mouse.x + 0.5, mouse.y + 0.5), z);
                    scene.particleInteractor.update(position);
                });
            }
        });

        addMouseReleasedMapping(Qt.LeftButton, function(mouse, viewer) {
            if(scene.particleInteractor)
                scene.particleInteractor.release();

            if(selectedManipulator && selectedManipulator.mouseReleased)
                selectedManipulator.mouseReleased(mouse, viewer);

            scene.selectedManipulator = null;

            setMouseMoveMapping(null);
        });

    }
}

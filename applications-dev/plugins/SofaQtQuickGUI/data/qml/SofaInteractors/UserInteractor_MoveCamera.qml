import QtQuick 2.0
import SofaBasics 1.0
import SceneComponent 1.0
import "qrc:/SofaCommon/SofaToolsScript.js" as SofaToolsScript

UserInteractor {
    id: root

    property int previousX: -1
    property int previousY: -1

    property real moveSpeed: 0.00133
    property real turnSpeed: 20.0
    property real zoomSpeed: 1.0

    function moveCamera_init() {

        addMouseDoubleClickedMapping(Qt.LeftButton, function(mouse, viewer) {
            var position = viewer.projectOnGeometry(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
            if(1.0 === position.w) {
                viewer.camera.target = position.toVector3d();
                viewer.crosshairGizmo.pop();
            }
        });

        addMousePressedMapping(Qt.RightButton, function(mouse, viewer) {
            previousX = mouse.x;
            previousY = mouse.y;

            viewer.crosshairGizmo.show();
            SofaToolsScript.Tools.overrideCursorShape = Qt.ClosedHandCursor;

            setMouseMoveMapping(function(mouse, viewer) {
                if(!viewer.camera)
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

                viewer.camera.turn(angleAroundX, angleAroundY, angleAroundZ);

                previousX = mouse.x;
                previousY = mouse.y;
            });
        });

        addMouseReleasedMapping(Qt.RightButton, function(mouse, viewer) {
            setMouseMoveMapping(null);

            SofaToolsScript.Tools.overrideCursorShape = 0;
            viewer.crosshairGizmo.hide();
        });

        addMouseDoubleClickedMapping(Qt.RightButton, function(mouse, viewer) {
            viewer.camera.alignCameraAxis();
        });

        addMousePressedMapping(Qt.MiddleButton, function(mouse, viewer) {
            previousX = mouse.x;
            previousY = mouse.y;

            viewer.crosshairGizmo.show();
            SofaToolsScript.Tools.overrideCursorShape = Qt.ClosedHandCursor;

            setMouseMoveMapping(function(mouse) {
                if(!viewer.camera)
                    return;

                var screenToScene = viewer.camera.target.minus(viewer.camera.eye()).length();

                var moveX = (mouse.x - previousX) * screenToScene * moveSpeed;
                var moveY = (mouse.y - previousY) * screenToScene * moveSpeed;
                viewer.camera.move(-moveX, moveY, 0.0);

                previousX = mouse.x;
                previousY = mouse.y;
            });
        });

        addMouseReleasedMapping(Qt.MiddleButton, function(mouse, viewer) {
            setMouseMoveMapping(null);

            SofaToolsScript.Tools.overrideCursorShape = 0;
            viewer.crosshairGizmo.hide();
        });

        setMouseWheelMapping(function(wheel, viewer) {
            
            if(!viewer.camera)
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

            viewer.camera.zoom(factor);

            wheel.accepted = true;
        });
        
        addMouseDoubleRightClickedMapping(Qt.RightButton, function(mouse) {
            // Align camera axis
            viewer.camera.alignCameraAxis();
        });
    }

    function init() {
        moveCamera_init();
    }

}

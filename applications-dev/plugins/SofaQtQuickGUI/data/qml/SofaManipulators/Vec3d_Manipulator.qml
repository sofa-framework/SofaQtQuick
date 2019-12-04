import QtQuick 2.0
import Vec3d_Manipulator 1.0

Vec3d_Manipulator {
    id: root

    function mouseMoved(mouse, sofaViewer) {
        console.error("###############   Manipulating with index " + index)

        manipulate(sofaViewer, mouse);
    }

    function mouseReleased(mouse, sofaViewer) {
        console.error("###############   Releasing...")
        index = -1;
    }
}

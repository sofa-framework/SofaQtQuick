import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import SofaBasics 1.0
import SofaApplication 1.0

SofaSceneInterface {
    id: root

    toolbar: Rectangle {
        width: 100
        height: 100
        color: "red"
    }

    toolpanel: ColumnLayout {
        height: implicitHeight

        Button {
            id: setOneCamera
            Layout.fillWidth: true

            text: "Set camera of the focused SofaViewer"

            onClicked: {
                var camera = SofaApplication.focusedSofaViewer ? SofaApplication.focusedSofaViewer.camera : null
                setCamera(camera);
            }
        }

        Button {
            id: setAllCameras
            Layout.fillWidth: true

            text: "Set camera of all SofaViewers"

            onClicked: {
                var cameras = SofaApplication.retrieveAllSofaViewerCameras();
                if(cameras)
                    for(var i = 0; i < cameras.length; ++i)
                        setCamera(cameras[i]);
            }
        }

        function setCamera(camera) {
            if(!camera)
                return;

            camera.lookAt(Qt.vector3d(0.0, 25.0, 100.0), Qt.vector3d(0.0, 25.0, 0.0), Qt.vector3d(0.0, 1.0, 0.0));
        }
    }
}

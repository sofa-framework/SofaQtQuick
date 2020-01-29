import QtQuick 2.12
import QtQuick.Dialogs 1.1

FileDialog {
    property alias sofaSceneFileUrl: control.fileUrl

    id: control
    modality: Qt.WindowModal
    nameFilters: ["SofaScene files (*.xml *.scn *.pyscn *.py *.simu *)"]
}

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import QtQuick.Layouts 1.12
import Qt.labs.folderlistmodel 2.12
import SofaColorScheme 1.0
import SofaWidgets 1.0
import QtQml 2.12
import QtQuick.Window 2.12
import Asset 1.0

Item {
    id: draggedData
    
    property string origin: "ProjectView"
    property Asset asset
    property string assetName: ""
    
    Drag.active: !fileIsDir ? mouseRegion.drag.active : false
    Drag.dragType: Drag.Automatic
    Drag.supportedActions: Qt.CopyAction
    Drag.mimeData: {
        "text/plain": "Copied text"
    }
}

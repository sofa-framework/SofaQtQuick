import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaSceneListModel 1.0
import SofaViews 1.0
import SofaWidgets 1.0

Component {
    id: sofaDataListViewWindowComponent
    
    Window {
        id: root
        width: 400
        height: 600
        modality: Qt.NonModal
        flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
        visible: true
        color: "lightgrey"
        
        Component.onCompleted: {
            width = Math.max(width, Math.max(loader.implicitWidth, loader.width));
            height = Math.min(height, Math.max(loader.implicitHeight, loader.height));
        }
        
        title: sofaComponent ? ("Data of component: " + sofaComponent.getName()) : "No component to visualize"
        
        property var sofaScene: root.sofaScene
        property var sofaComponent: sofaScene ? sofaScene.selectedComponent : null
        
        Loader {
            id: loader
            anchors.fill: parent
            
            sourceComponent: SofaDataListView {
                
                sofaScene: root.sofaScene
                sofaComponent: root.sofaComponent
            }
        }
    }
}

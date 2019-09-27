import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaWidgets 1.0
import Sofa.Core.SofaNode 1.0
import QtQml 2.3

Menu {
    id: assetMenu

    property var asset
    property string assetName: ""
    property var parentNode
    property var basemodel
    property var sceneModel
    property var sofaScene
    property var treeView
    property var selection

    title: "Asset Content"
    onAssetChanged: {
        if (assetMenu.asset) {
            menuRepeater.model = assetMenu.asset.scriptContent
            if (menuRepeater.model.length)
                enabled = true
            if (menuRepeater.model.count)
                enabled = true
        }
    }

    onOpened: {
        if (assetMenu.asset) {
            menuRepeater.model = assetMenu.asset.scriptContent
            if (menuRepeater.model.length)
                enabled = true
            if (menuRepeater.model.count)
                enabled = true
        }
    }

    MenuSeparator {}


    Repeater {
        id: menuRepeater

        Component {
            id: prefabSubmenuComponent
            PrefabSubMenu {}
        }

        Loader {
            id: assetLoader
            sourceComponent: prefabSubmenuComponent


            onLoaded: {
                item.model = modelData
                assetMenu.addMenu(item)
            }
        }
    }

}

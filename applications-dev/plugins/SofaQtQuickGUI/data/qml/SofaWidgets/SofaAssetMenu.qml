import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaWidgets 1.0
import Sofa.Core.SofaNode 1.0

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
            if (menuRepeater.model.count)
                enabled = true
        }
    }

    onOpened: {
        if (assetMenu.asset) {
            menuRepeater.model = assetMenu.asset.scriptContent
            if (menuRepeater.model.count)
                enabled = true
        }
    }

    Repeater {
        id: menuRepeater
        MenuItem {
            text: modelData.name
            onTextChanged: {
                if (text === "createScene")
                    asset.isSceneFile = true
                else asset.isSceneFile = false
            }

            icon.source: (modelData.type  === "function" && modelData.name === "createScene" ? "qrc:/icon/ICON_PYSCN.png" :
                         (modelData.type === "class" ? "qrc:/icon/ICON_PYTHON.png" :
                         (modelData.type === "SofaPrefab" ? "qrc:/icon/ICON_PREFAB.png" :
                         (modelData.type === "PythonScriptController" ? "qrc:/icon/ICON_PYController.png" :
                         (modelData.type === "PythonScriptDataEngine" ? "qrc:/icon/ICON_PYEngine.png" : "qrc:/icon/ICON_PYTHON.png")))))
            onTriggered: {
                if (!parentNode)
                    parentNode = sofaScene.root()
                if (!parentNode.isNode())
                    parentNode = parentNode.getFirstParent()
                assetName = modelData.name
                var p = createAsset()
                if (!p)
                    return
                if (basemodel) {
                    var srcIndex = basemodel.getIndexFromBase(p)
                    var index = sceneModel.mapFromSource(srcIndex);
                    treeView.collapseAncestors(index)
                    treeView.expandAncestors(index)
                    treeView.expand(index)
                    treeView.selection.setCurrentIndex(index, selection)
                }
            }

            function createAsset() {
                var newNode = asset.create(assetName)
                var hasNodes = newNode.getChildren().size()
                console.error("ParentNode address: " + parentNode)
                console.error("ParentNode Name: " + parentNode.getName())
                newNode.copyTo(parentNode)
                if (hasNodes) {
                    var childsList = parentNode.getChildren()
                    if (childsList.size() !== 0) {
                        return childsList.last()
                    }
                }
                return parentNode
            }

            ToolTip {
                text: modelData.docstring
            }
            Component.onCompleted: {
                assetName = modelData.name
            }
        }
    }
}

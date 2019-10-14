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
    id: menuId
    property var model

    title: model.name
    onTitleChanged: {
        console.log(model.name)
        if (title === "createScene")
            asset.isSceneFile = true
        else asset.isSceneFile = false
    }

    function getIconSource() {
        return (model.type  === "SofaScene" && model.name === "createScene" ? "qrc:/icon/ICON_PYSCN.png" :
                                                                              (model.type === "class" ? "qrc:/icon/ICON_PYTHON.png" :
                                                                                                        (model.type === "SofaPrefab" ? "qrc:/icon/ICON_PREFAB.png" :
                                                                                                                                       (model.type === "PythonScriptController" ? "qrc:/icon/ICON_PYController.png" :
                                                                                                                                                                                  (model.type === "PythonScriptDataEngine" ? "qrc:/icon/ICON_PYEngine.png" : "qrc:/icon/ICON_PYTHON.png")))))
    }
    delegate: MenuItem {
        id: menuItemDelegate
        icon.source: getIconSource()

    }
    MenuItem {
        text: "Create instance"
        icon.source: getIconSource()

        onTriggered: {
            if (!parentNode)
                parentNode = sofaScene.root()
            if (!parentNode.isNode())
                parentNode = parentNode.getFirstParent()
            assetName = model.name
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
            var newNode = asset.create(parentNode, assetName)
            return newNode
        }

        ToolTip {
            text: model.docstring
        }
    }
    MenuItem {
        text: "Custom params..."
        icon.source: "qrc:/icon/ICON_GEAR.png"

        onTriggered: {
            if (!parentNode)
                parentNode = sofaScene.root()
            if (!parentNode.isNode())
                parentNode = parentNode.getFirstParent()
            assetName = model.name
            model.openSettings()
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
            var newNode = asset.create(parentNode, assetName)
            return newNode
        }

        ToolTip {
            text: model.docstring
        }
    }
}

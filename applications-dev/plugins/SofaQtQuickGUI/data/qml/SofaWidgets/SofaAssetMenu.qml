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

    property var model: null
    property string assetName: ""
    property var draggedData: null
    property var parentNode: null
    property var basemodel: null
    property var sceneModel: null
    property var sofaScene: null
    property var treeView: null
    property var selection: null

    title: "Asset Content"
    visible: true
    Repeater {
        model: assetMenu.model
        MenuItem {
            text: modelData.name
            onTextChanged: {
                if (text === "createScene")
                    wrapper.isSceneFile = true
                else wrapper.isSceneFile = false
            }

            icon.source: (modelData.type  === "function" && modelData.name === "createScene" ? "qrc:/icon/ICON_PYSCN.png" :
                         (modelData.type === "class" ? "qrc:/icon/ICON_PYTHON.png" :
                         (modelData.type === "SofaPrefab" ? "qrc:/icon/ICON_PREFAB.png" :
                         (modelData.type === "PythonScriptController" ? "qrc:/icon/ICON_PYController.png" :
                         (modelData.type === "PythonScriptDataEngine" ? "qrc:/icon/ICON_PYEngine.png" : "qrc:/icon/ICON_PYTHON.png")))))
            onTriggered: {
                assetName = modelData.name
                var parentNode = createAsset()
                var srcIndex = basemodel.getIndexFromBase(parentNode)
                var index = sceneModel.mapFromSource(srcIndex);
                treeView.collapseAncestors(index)
                treeView.expandAncestors(index)
                treeView.expand(index)
                treeView.selection.setCurrentIndex(index, selection)
            }
            Component.onCompleted: {
                assetName = modelData.name
            }
        }
    }
    function createAsset()
    {
        var asset = draggedData.getAsset(assetName)
        var parent = sofaScene.root()
        if (parentNode !== "")
            parent = parent.getNodeInGraph(parentNode)
        asset.copyTo(parent)
        return parent.getChildren().last()
    }
}

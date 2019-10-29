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
    property string assetName: asset ? asset.getTypeString() : "()"
    property var parentNode
    property var basemodel
    property var sceneModel
    property var sofaScene
    property var treeView
    property var selection

    title: "Asset Content " + assetName

    onAssetChanged: {
        if (assetMenu.asset) {
            menuRepeater.model = assetMenu.asset.scriptContent
            if (menuRepeater.model === undefined) {
                enabled = false
                visible = false
            }
            else if (menuRepeater.model.length) {
                enabled = true
            }
            else if (menuRepeater.model.count) {
                enabled = true
            }
            else {
                enabled = false
                visible = false
            }
        }
    }

    MenuSeparator {}
    Repeater {
        id: menuRepeater

        delegate : MenuItem {
            text: modelData["name"]
            icon.source: getIconSource(modelData["type"])

            function getIconSource(type)
            {
                if(type  === "SofaScene")
                    return "qrc:/icon/ICON_PYSCN.png";
                if (type === "class")
                    return "qrc:/icon/ICON_PYTHON.png";
                if (type === "SofaPrefab")
                    return "qrc:/icon/ICON_PREFAB.png";
                if (type === "PythonScriptController")
                    return "qrc:/icon/ICON_PYController.png";
                if (type === "PythonScriptDataEngine")
                    return "qrc:/icon/ICON_PYEngine.png"
                return "qrc:/icon/ICON_PYTHON.png";
            }


            function doCreate() {
                var p = asset.create(parentNode, text)
                if (!p){
                    console.log("Unable to create and asset for: ", text)
                    return
                }

                if (basemodel) {
                    var srcIndex = basemodel.getIndexFromBase(p)
                    var index = sceneModel.mapFromSource(srcIndex);
                    treeView.collapseAncestors(index)
                    treeView.expandAncestors(index)
                    treeView.expand(index)
                    treeView.selection.setCurrentIndex(index, selection)
                }
            }

            onTriggered:
            {
                assetMenu.visible=false
                if(parentNode.isPrefab()){
                    messageDialog.onYes.connect( doCreate )
                    messageDialog.open()
                    return
                }
                doCreate()
            }
        }
    }

    MessageDialog
    {
        visible: false
        id: messageDialog
        title: "Error"
        text: "This will break the existing prefab. Are you sure ? "
        icon: StandardIcon.Critical
        standardButtons: StandardButton.Yes | StandardButton.No
    }
}


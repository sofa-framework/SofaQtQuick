import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaWidgets 1.0

Menu {
    id: assetMenu

    property var model: null
    property var draggedData: null
    property var parentNode: null
    property var sofaScene: null
    title: "Asset Content"
    visible: true
    Repeater {
        model: assetMenu.model
        MenuItem {
            text: modelData.name
            icon.source: (modelData.type  === "function" && modelData.name === "createScene" ? "qrc:/icon/ICON_PYSCN.png" :
                         (modelData.type === "class" ? "qrc:/icon/ICON_PYTHON.png" :
                         (modelData.type === "SofaPrefab" ? "qrc:/icon/ICON_PREFAB.png" :
                         (modelData.type === "PythonScriptController" ? "qrc:/icon/ICON_PYController.png" :
                         (modelData.type === "PythonScriptDataEngine" ? "qrc:/icon/ICON_PYEngine.png" : "qrc:/icon/ICON_PYTHON.png")))))
            onTriggered: {
                console.error("Parent node for asset is " + parentNode)
                var asset = draggedData.getAsset(modelData.name)
                sofaScene.addExistingNodeTo(parentNode === "" ? sofaScene.node("/") : sofaScene.node(parentNode), asset)
            }
        }
    }
}

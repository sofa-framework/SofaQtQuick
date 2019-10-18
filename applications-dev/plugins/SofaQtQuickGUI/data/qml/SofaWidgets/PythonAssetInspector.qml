import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import Asset 1.0
import PythonAsset 1.0
import HighlightComponent 1.0

Item {
    id: root
    property Asset selectedAsset

    ScrollView {
        anchors.top: root.top
        width: root.parent.width
        height: root.parent.height


        Column {
            id: columnID
            spacing: 10
            Repeater {
                model: selectedAsset.scriptContent
                GroupBox {
                    id: gbox
                    width: root.parent.width
                    implicitHeight: rectId.height + (gbox.expanded ? 30 : 20)
                    title: modelData.name
                    titleIcon: (modelData.type === "SofaScene" && modelData.name === "createScene" ? "qrc:/icon/ICON_PYSCN.png" :
                                                                                                     (modelData.type === "Class" ? "qrc:/icon/ICON_PYTHON.png" :
                                                                                                                                   (modelData.type === "SofaPrefab" ? "qrc:/icon/ICON_PREFAB.png" :
                                                                                                                                                                      (modelData.type === "Controller" ? "qrc:/icon/ICON_PYController.png" :
                                                                                                                                                                                                         (modelData.type === "DataEngine" ? "qrc:/icon/ICON_PYEngine.png" : "qrc:/icon/ICON_PYTHON.png")))))
                    buttonIconSource: modelData.type !== "SofaScene" ? "qrc:/icon/ICON_GEAR.png" : undefined
                    buttonAction: modelData.type !== "SofaScene" ? modelData.openSettings : undefined
                    Rectangle {
                        id: rectId
                        color: 'transparent'
                        width: root.parent.width / 4 * 3
                        implicitHeight: gbox.expanded ? codeArea.implicitHeight : 0
                        TextArea {
                            id: codeArea
                            anchors.fill: parent
                            text: modelData.sourcecode
                            wrapMode: Text.Wrap
                            readOnly: true
                            hoverEnabled: true
                            ToolTip {
                                text: modelData.type
                                description: modelData.docstring
                            }

                            HighlightComponent {
                                id: testTextArea
                                syntax: "py"
                                Component.onCompleted: testTextArea.onCompleted()
                            }
                        }
                    }
                    MouseArea {
                        id: mouseId
                        anchors.fill: parent
                        drag.target: draggedData
                        drag.onActiveChanged: {
                            if (drag.active) {
                                draggedData.asset = selectedAsset
                                draggedData.assetName = modelData.name
                            }
                        }
                        onClicked: parent.clicked()
                    }

                    DraggableAssetItem {
                        id: draggedData
                        origin: "Inspector"
                        Drag.active: mouseId.drag.active
                    }
                }
            }
        }
    }
}

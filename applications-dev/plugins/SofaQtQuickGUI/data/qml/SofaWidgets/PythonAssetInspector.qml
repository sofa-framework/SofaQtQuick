import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0
import HighlightComponent 1.0
import SofaBasics 1.0
import SofaApplication 1.0
import Asset 1.0

Item {
    id: root
    property Asset selectedAsset
    anchors.fill: parent
    anchors.topMargin: 10
    Flickable {
        id: scrollview
        anchors.fill: parent
        clip: true

        flickableDirection: Flickable.VerticalFlick
        boundsMovement: Flickable.StopAtBounds
        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AlwaysOff
        }

        ScrollBar.vertical: VerticalScrollbar {
            id: scrollbar
            content: scrollview.contentItem
        }

        contentHeight: columnID.height

        ColumnLayout {
            id: columnID
            width: parent.width - 10
            spacing: 10
            Repeater {
                Layout.fillWidth: true
                model: selectedAsset.scriptContent
                GroupBox {
                    id: gbox
                    Layout.fillWidth: true
                    implicitHeight: rectId.height + (gbox.expanded ? 30 : 20)
                    title: modelData.name
                    titleIcon: {
                        if (modelData.type === "SofaScene" && modelData.name === "createScene")
                            return "qrc:/icon/ICON_PYSCN.png"
                        else if (modelData.type === "Class")
                            return "qrc:/icon/ICON_PYTHON.png"
                        else if (modelData.type === "SofaPrefab")
                            return "qrc:/icon/ICON_PREFAB.png"
                        else if (modelData.type === "Controller")
                            return "qrc:/icon/ICON_PYController.png"
                        else if (modelData.type === "DataEngine")
                            return "qrc:/icon/ICON_PYEngine.png"
                        else
                            return "qrc:/icon/ICON_PYTHON.png"
                    }
                    buttonIconSource: "qrc:/icon/edit.png"
                    onButtonClicked: {
                        SofaApplication.openInEditor(selectedAsset.path, Number(modelData.lineno)+1)
                    }

                    Rectangle {
                        id: rectId
                        color: "transparent"
                        implicitWidth: parent.width
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
                        onClicked: {
                            forceActiveFocus()
                        }
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
    MouseArea {
        anchors.fill: parent
        onClicked: {
            forceActiveFocus()
        }
    }
}

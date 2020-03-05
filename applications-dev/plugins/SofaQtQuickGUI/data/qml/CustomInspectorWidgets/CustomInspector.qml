import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import SofaApplication 1.0
import SofaBasics 1.0




ColumnLayout {
    id: root
    spacing: 10
    property var component: SofaApplication.selectedComponent

    property var showAll: true
    property var dataDict: {"Base": ["name", "componentState", "printLog"]}
    onDataDictChanged: {
        groupRepeater.model = Object.keys(dataDict).length
    }
    property var linkList: []
    property var infosDict: {"class":component.getClassName(), "name": component.getName()}
    property var labelWidth: 1

    Repeater {
        id: groupRepeater
        model: Object.keys(dataDict).length

        GroupBox {
            title: Object.keys(dataDict)[index]
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 1
                anchors.fill: parent
                Repeater {
                    id: dataRepeater
                    model: dataDict[Object.keys(dataDict)[index]]
                    Layout.fillWidth: true
                    RowLayout {
                        id: sofaDataLayout
                        property var sofaData: modelData ? component.getData(modelData) : null
                        Layout.fillWidth: true

                        Label {
                            Layout.minimumWidth: root.labelWidth

                            text:  modelData
                            color: sofaDataLayout.sofaData.properties.required ? "red" : "black"
                            ToolTip {
                                text: sofaDataLayout.sofaData.getName()
                                description: sofaDataLayout.sofaData.getHelp()
                                visible: marea.containsMouse
                            }

                            MouseArea {
                                id: marea
                                anchors.fill: parent
                                hoverEnabled: true
                                onPressed: forceActiveFocus()
                                DropArea {
                                    id: dropArea
                                    keys: ["text/plain"]
                                    anchors.fill: parent
                                    onDropped: {
                                        if (drag.source.item.getPathName() === SofaApplication.selectedComponent.getPathName()) {
                                            console.error("Cannot link datafields to themselves")
                                            return;
                                        }
                                        var data = drag.source.item.getData(sofaData.getName())
                                        if (data !== null) {
                                            sofaData.parent = (data)
                                            sofaData.value = data.value
                                        }
                                    }
                                }
                            }
                            Component.onCompleted: {
                                if (width > root.labelWidth)
                                    root.labelWidth = width
                            }
                        }
                        Loader {
                            id: dataItemLoader

                            Layout.fillWidth: true
                            source: linkButton.checked
                                    ? "qrc:/SofaBasics/SofaLinkItem.qml"
                                    : "qrc:/SofaDataTypes/SofaDataType_" + sofaDataLayout.sofaData.properties.type + ".qml"
                            onLoaded: {
                                item.sofaData = Qt.binding(function(){return sofaDataLayout.sofaData})
                            }
                            DropArea {
                                id: dropArea2
                                anchors.fill: parent

                                onDropped: {
                                    data = drag.source.item.findData(sofaData.getName())
                                    if (drag.source.item.getPathName() === SofaApplication.selectedComponent.getPathName()) {
                                        console.error("Cannot link datafields to themselves")
                                        return;
                                    }

                                    if (data !== null) {
                                        sofaData.parent = data
                                        var v = sofaData.value
                                    }
                                }
                            }


                        }
                        Button {
                            id: linkButton
                            Layout.preferredWidth: 14
                            Layout.preferredHeight: 14
                            anchors.margins: 3
                            checkable: true
                            checked: sofaDataLayout.sofaData ? null !== sofaDataLayout.sofaData.parent : false
                            onCheckedChanged: {
                                if (dataItemLoader.item)
                                    dataItemLoader.item.forceActiveFocus()
                            }

                            activeFocusOnTab: false
                            ToolTip {
                                text: "Show / Hide link to parent"
                            }

                            Image {
                                id: linkButtonImage
                                anchors.fill: parent
                                source: sofaDataLayout.sofaData.parent ? "qrc:/icon/validLink.png" : "qrc:/icon/invalidLink.png"
                            }
                        }
                    }
                }
            }
        }
    }
    GroupBox {
        title: "Links"
        visible: linkList.length > 0
        ColumnLayout {
            anchors.fill: parent
            spacing: 1
            Repeater {
                model: linkList
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text: modelData
                        Layout.minimumWidth: root.labelWidth
                        Component.onCompleted: {
                            if (width > root.labelWidth)
                                root.labelWidth = width
                        }
                        color: "black"
                        MouseArea {
                            anchors.fill: parent
                            onPressed: forceActiveFocus()
                        }
                    }
                    TextField {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        readOnly: true

                        text: component.findLink(modelData).getLinkedPath().trim()
                    }
                    Rectangle {
                        color: "transparent"
                        width: 14
                        MouseArea {
                            anchors.fill: parent
                            onPressed: forceActiveFocus()
                        }
                    }
                }
            }
        }

    }
    GroupBox {
        title: "Infos"
        ColumnLayout {
            id: infosLayout
            property var labelWidth: 0

            Repeater {
                model: Object.keys(infosDict)
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.minimumWidth: infosLayout.labelWidth

                        text: modelData + " :"
                        color: "black"
                        Component.onCompleted: {
                            if (width > infosLayout.labelWidth)
                                infosLayout.labelWidth = width
                        }
                        MouseArea {
                            anchors.fill: parent
                            onPressed: forceActiveFocus()
                        }
                    }
                    Label {
                        text: infosDict[Object.keys(infosDict)[index]]
                        color: "#333333"
                        elide: Qt.ElideRight
                        clip: true
                        MouseArea {
                            anchors.fill: parent
                            onPressed: forceActiveFocus()
                        }
                    }
                }
            }
        }

    }
}

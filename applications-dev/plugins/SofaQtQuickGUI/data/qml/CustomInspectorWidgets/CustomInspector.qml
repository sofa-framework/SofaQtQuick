import QtQuick 2.5
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import SofaApplication 1.0
import SofaBasics 1.0

ColumnLayout {
    id: root
    anchors.fill: parent
    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 10

    property var showAll: true
    property var component: SofaApplication.selectedComponent
    property var dataDict: {"Base": ["name", "componentState", "printLog"]}
    property var linkList: []
    property var infosDict: {"class":component.getClassName(), "name": component.getName()}
    onDataDictChanged: {
        groupRepeater.model = Object.keys(dataDict).length
    }

    property var labelWidth: 1

    Repeater {
        id: groupRepeater
        model: Object.keys(dataDict).length

        GroupBox {
            title: Object.keys(dataDict)[index]
            ColumnLayout {
                spacing: 1
                anchors.fill: parent
                Repeater {
                    id: dataRepeater
                    model: dataDict[Object.keys(dataDict)[index]]

                    RowLayout {
                        id: sofaDataLayout
                        property var sofaData: modelData ? component.getData(modelData) : null

                        Label {
                            Layout.minimumWidth: root.labelWidth

                            text:  modelData
                            color: "black"
                            ToolTip {
                                text: sofaDataLayout.sofaData.getName()
                                description: sofaDataLayout.sofaData.getHelp()
                                visible: marea.containsMouse
                            }
                            MouseArea {
                                id: marea
                                anchors.fill: parent
                                hoverEnabled: true
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
                                item.sofaData = sofaDataLayout.sofaData
                            }
                        }
                        Button {
                            id: linkButton
                            Layout.preferredWidth: 14
                            Layout.preferredHeight: 14
                            anchors.margins: 3
                            checkable: true
                            checked: sofaData ? null !== sofaData.getParent() : false
                            onCheckedChanged: {
                                if (dataItemLoader.item)
                                    dataItemLoader.item.forceActiveFocus()
                            }

                            activeFocusOnTab: false
                            ToolTip {
                                text: "Link the data to another one."
                            }

                            Image {
                                id: linkButtonImage
                                anchors.fill: parent
                                source: linkButton.checked && sofaDataLayout.sofaData.getParent() ? "qrc:/icon/linkValid.png" : "qrc:/icon/link.png"
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
                    }
                    Label {
                        text: infosDict[Object.keys(infosDict)[index]]
                        color: "#333333"
                        elide: Qt.ElideRight
                        clip: true
                    }
                }
            }
        }
    }
}

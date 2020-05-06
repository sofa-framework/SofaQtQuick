/*
Copyright 2016, CNRS

Author: damien.marchal@univ-lille1.fr

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaColorScheme 1.0
import QtGraphicalEffects 1.12
import SofaApplication 1.0
import SofaComponent 1.0
import SofaInspectorDataListModel 1.0
import SofaWidgets 1.0


Item {
    id: root
    anchors.fill: parent
    property var selectedAsset: SofaApplication.currentProject.selectedAsset

    Rectangle {
        id: topRect
        color: SofaApplication.style.contentBackgroundColor
        anchors.fill: parent
        visible: selectedAsset === null

        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            Rectangle {
                Layout.preferredHeight: 20
                Layout.fillWidth: true
                color: "transparent"
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        print("Mouse area pressed in a rectangle.")
                        root.forceActiveFocus()
                    }
                }


                DropArea {
                    id: dropArea
                    anchors.fill: parent
                    onDropped: {
                        /// Drop of a SofaBase form the hierachy. This indicate that we want
                        /// to automatically connect all the data fields with similar names.
                        if (drag.source.origin === "Hierarchy") {
                            var droppedItem = drag.source.item
                            /// Ecmascript6.0 'for..of' is valid, don't trust qtcreator there is an error
                            if (droppedItem.getPathName() === SofaApplication.selectedComponent.getPathName()) {
                                console.error("Cannot link datafields to themselves")
                                return;
                            }
                            for (var fname of droppedItem.getDataFields())
                            {
                                var data = SofaApplication.selectedComponent.findData(fname);
                                if (data !== null && data.isAutoLink())
                                {
                                    data.setValue(droppedItem.getData(fname).getValue())
                                    data.setParent(droppedItem.getData(fname))
                                }
                            }
                        }
                    }
                }
                RowLayout {
                    z: 3
                    id: header
                    anchors.fill: parent
                    anchors.rightMargin: 10

                    Text {
                        id: detailsArea
                        Layout.fillWidth: true
                        text : "Details " + ((SofaApplication.selectedComponent===null)? "" : "("+ SofaApplication.selectedComponent.getClassName() + ")")
                        color: "black"
                    }
                    Label {
                        id: showAllLabel
                        text: "Show all: "
                        color: "black"
                    }
                    CheckBox {
                        id : showAll
                    }
                    Button {
                        id : customizeButton
                        hoverEnabled: true
                        ColorImage {
                            width: 13
                            height: 13
                            anchors.centerIn: parent
                            source: "qrc:/icon/edit.png"
                            fillMode: Image.PreserveAspectFit
                            color: customizeButton.hovered ? "darkgrey" : "#393939"
                        }
                        implicitWidth: 20
                        implicitHeight: 20
                        onClicked: {
                            var file = SofaApplication.inspectorsDirectory() + SofaApplication.selectedComponent.getClassName() + ".qml";
                            SofaApplication.createInspector(file)
                            SofaApplication.openInEditor(file)
                        }
                    }
                }
            }


            Flickable {
                id: scrollview
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.leftMargin: 5
                clip: true
                flickableDirection: Flickable.VerticalFlick
                boundsMovement: Flickable.StopAtBounds
                ScrollBar.horizontal: ScrollBar {
                    policy: ScrollBar.AlwaysOff
                }

                ScrollBar.vertical: VerticalScrollbar {
                    id: scrollbar
                    implicitWidth: 12
                    content: content
                }
                MouseArea {
                    anchors.fill: parent
                    onPressed: forceActiveFocus()
                }


                contentWidth: content.width; contentHeight: content.height
                ColumnLayout {
                    id: content
                    width: scrollbar.policy === ScrollBar.AlwaysOn ? scrollview.width - 20 : scrollview.width - 12
                    x: 12
                    Loader {
                        Layout.fillWidth: true
                        id: customInspectorLoader
                        function getWidget(component)
                        {
                            if (!component)
                                return null

                            if (showAll.checked)
                                return Qt.createComponent("qrc:/CustomInspectorWidgets/BaseInspector.qml")

                            var ui = Qt.createComponent(SofaApplication.inspectorsDirectory() + component.getClassName() + ".qml")
                            if (ui.status === Component.Ready) {
                                return ui
                            }
                            else {
                                var list = component.getInheritedClassNames()

                                for (var i = 0 ; i < list.length; ++i) {
                                    ui = Qt.createComponent(SofaApplication.inspectorsDirectory() + list[i] + ".qml")
                                    if (ui.status === Component.Ready) {
                                        return ui
                                    }
                                }
                            }
                            return Qt.createComponent("qrc:/CustomInspectorWidgets/BaseInspector.qml")
                        }
                        sourceComponent: getWidget(SofaApplication.selectedComponent)
                        onLoaded: {
                            item.showAll = Qt.binding(function(){ return showAll.checked})
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: assetArea
        width: parent.width
        height: parent.height
        color: "transparent"
        visible: selectedAsset !== null

        Loader {
            anchors.fill: parent
            id: assetLoaderId
            source: root.selectedAsset ? root.selectedAsset.getAssetInspectorWidget() : ""
            onLoaded: {
                if (item) {
                    if (item instanceof DynamicContent_Error)
                        item.errorMessage = "No inspector widget for this asset type"
                    else {
                        item.selectedAsset = Qt.binding(function() {
                            if (root.selectedAsset)
                                return root.selectedAsset
                            else return null
                        })
                    }
                    item.parent = assetArea
                }
            }
        }
    }
}

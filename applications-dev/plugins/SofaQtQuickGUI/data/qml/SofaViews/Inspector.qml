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
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        visible: selectedAsset === null
        DropArea {
            id: dropArea
            anchors.fill: parent
            onDropped: {
                /// Drop of a SofaBase form the hierachy. This indicate that we want
                /// to automatically connect all the data fields with similar names.
                if (drag.source.origin === "Hierarchy") {
                    var droppedItem = drag.source.item 
                    /// Ecmascript6.0 'for..of' is valid, don't trust qtcreator
                    for(var fname of droppedItem.getDataFields())
                    {
                        var data = SofaApplication.selectedComponent.findData(fname);
                        if( data !== null && data.isAutoLink())
                        {
                            data.setParent(droppedItem.getData(fname))
                            console.log("FNAME" + fname + "-> " + data)
                        }
                    }
                }
            }
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            RowLayout {
                z: 3
                id: header
                Layout.fillWidth: true
                Layout.preferredHeight: 20

                Text {
                    id: detailsArea
                    Layout.fillWidth: true
                    text : "Details " + ((SofaApplication.selectedComponent===null)? "" : "("+ SofaApplication.selectedComponent.getClassName() + ")")
                    color: "black"
                }
                Label {
                    id: showAllLabel
//                    anchors.right: isDebug.left
//                    anchors.verticalCenter: header1.verticalCenter
                    text: "Show all: "
                    color: "black"
                }
                CheckBox {
                    id : showAll
//                    anchors.right: header1.right
//                    anchors.verticalCenter: header1.verticalCenter
                }
            }

            ScrollView {
                id: scrollview
                Layout.fillHeight: true
                Layout.fillWidth: true
                /*
                ScrollBar.vertical: ScrollBar {
                    id: scrollbar
                    policy: scrollview.height > scrollview.contentHeight ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
                    parent: scrollview
                    x: scrollview.mirrored ? 0 : scrollview.width - width
                    size: 0.3
                    active: true
                    contentItem: GBRect {
                        implicitWidth: 12
                        implicitHeight: 100
                        radius: 6
                        border.color: "#3f3f3f"
                        LinearGradient {
                            cached: true
                            source: parent
                            anchors.left: parent.left
                            anchors.leftMargin: 1
                            anchors.right: parent.right
                            anchors.rightMargin: 1
                            anchors.top: parent.top
                            anchors.topMargin: 0
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 0

                            start: Qt.point(0, 0)
                            end: Qt.point(12, 0)
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#979797" }
                                GradientStop { position: 1.0; color: "#7b7b7b" }
                            }
                        }
                        isHorizontal: true
                        borderGradient: Gradient {
                            GradientStop { position: 0.0; color: "#444444" }
                            GradientStop { position: 1.0; color: "#515151" }
                        }
                    }

                    background: GBRect {
                        border.color: "#3f3f3f"
                        radius: 6
                        implicitWidth: 12
                        implicitHeight: scrollview.height
                        LinearGradient {
                            cached: true
                            source: parent
                            anchors.left: parent.left
                            anchors.leftMargin: 1
                            anchors.right: parent.right
                            anchors.rightMargin: 1
                            anchors.top: parent.top
                            anchors.topMargin: 0
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 0
                            start: Qt.point(0, 0)
                            end: Qt.point(12, 0)
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#565656" }
                                GradientStop { position: 1.0; color: "#5d5d5d" }
                            }
                        }
                        isHorizontal: true
                        borderGradient: Gradient {
                            GradientStop { position: 0.0; color: "#444444" }
                            GradientStop { position: 1.0; color: "#515151" }
                        }
                    }
                }
*/


                ColumnLayout {
                    width: scrollview.width

                    Loader {
                        Layout.fillWidth: true
                        id: customInspectorLoader
                        function getWidget(component)
                        {
                            if (showAll.checked)
                                return Qt.createComponent("qrc:/CustomInspectorWidgets/BaseInspector.qml")

                            var ui = Qt.createComponent("qrc:/CustomInspectorWidgets/" + component.getClassName() + "Inspector.qml")
                            if (ui.status === Component.Ready)
                            {
                                return ui
                            }
                            else {
                                var list = component.getInheritedClassNames()

                                for (var i = 0 ; i < list.length; ++i) {
                                    ui = Qt.createComponent("qrc:/CustomInspectorWidgets/" + list[i] + "Inspector.qml")
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
        height: parent.height - 42
        color: "transparent"
        visible: selectedAsset !== null

        Loader {
            id: assetLoaderId
            source: root.selectedAsset ? root.selectedAsset.getAssetInspectorWidget() : ""
            onLoaded: {
                if (item) {
                    console.error(item)
                    item.selectedAsset = Qt.binding(function() {
                        if (root.selectedAsset)
                            return root.selectedAsset
                        else return null
                    })
                    item.parent = assetArea
                }
            }
        }
    }
}

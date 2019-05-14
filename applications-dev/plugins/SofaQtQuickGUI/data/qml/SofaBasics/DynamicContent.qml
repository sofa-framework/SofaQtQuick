/*******************************************************************
Copyright 2015, Anatoscope
Copyright 2019, CNRS

This file is part of sofaqtquick.

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
*******************************************************************/
/*******************************************************************
 * Contributors:
 *   - Olivier Carre (Anatoscope) initial version
 *   - damien.marchal@univ-lille.fr (CNRS) deep refactoring
 *******************************************************************/
import QtQuick 2.5
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import SofaApplication 1.0
import SofaViewListModel 1.0
import SofaBasics 1.0
import SofaColorScheme 1.0

import LiveFileMonitorSingleton 1.0


//TODO(dmarchal 10/01/2019): move the file model into a separated file
//TODO(dmarchal 10/01/2019): move the drop down menu into a separated file

/// DynamicContent is a widget that contains one of the SofaViews
/// with a menu to select which is the view to display.
Item {

    /// Refreshing the view every time a file is modified
    //property var files : LiveFileMonitorSingleton.files
    //onFilesChanged: {
    //if (checkBoxEditting.checked)
    //    loaderLocation.refresh(comboBox.model.get(comboBox.currentIndex))
    //}


    //TODO(dmarchal: 10/01/2019 move that into an utilitary file)
    //TODO(dmarchal: 28/01/2019 unify the function so it can work with any object having a "lenght" property
    function findIndex(model, criteria) {
        for(var i = 0; i < model.count(); ++i) { if (criteria(model.get(i))) return i }
        return null
    }

    /// SofaViewListModel is a Singleton that hold the list of views (name, path)
    /// to expose into the view selector. The model is implemented in C++
    /// and inherit from QAbstractItemModel
    property QtObject listModel: SofaViewListModel

    property string defaultContentName
    property string currentContentName
    property string sourceDir: "qrc:/SofaViews"
    property int    contentUiId: 0
    property var    properties

    property int uiId: bootstrap()
    property int previousUiId: uiId

    readonly property alias contentItem: loaderLocation.contentItem
    readonly property bool isDynamicContent: true

    id: root
    clip: true
    anchors.fill: parent

    onUiIdChanged:
    {
        SofaApplication.uiSettings.replace(previousUiId, uiId);
    }

    /// This stores the setting for this view.
    Settings {
        id: uiSettings
        category: 0 !== root.uiId ? "ui_" + root.uiId : "dummy"

        property alias sourceDir : root.sourceDir
        property alias defaultContentName : root.defaultContentName
        property alias currentContentName : root.currentContentName
        property alias contentUiId : root.contentUiId
    }

    /// Initialize the view.
    function bootstrap()
    {
        // If the view is created from one that already has a parent set we duplicate
        /// it.
        if(parent && undefined !== parent.contentUiId && 0 !== parent.contentUiId)
        {
            root.uiId = parent.contentUiId;
            return;
        }

        /// Otherwise we create a new view ID.
        root.uiId = SofaApplication.uiSettings.generate();
    }

    Item {
        anchors.fill: parent

        /////////////////////////////////////////////////////////
        // The toolbar containing a dropdown menu and some
        // button and checkbox.
        /////////////////////////////////////////////////////////
        GBRect {
            id: toolBar
            color: "#757575"
            border.color: "black"

            borderWidth: 1
            borderGradient: Gradient {
                GradientStop { position: 0.0; color: "#7a7a7a" }
                GradientStop { position: 1.0; color: "#5c5c5c" }
            }
            height: 24
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            RowLayout {
                id: toolBarLayout
                y: 2
                spacing: 2
                ComboBox
                {
                    id: comboBox
                    textRole: "name"
                    model: listModel
                    sizeToContents: true
                    currentIndex: 0

                    onCurrentIndexChanged:
                    {
                        loaderLocation.refresh(listModel.get(currentIndex));
                        root.currentContentName = currentContentName;
                    }

                    //property var files : LiveFileMonitorSingleton.files
                    //onFilesChanged: {
                    //    loaderLocation.refresh(listModel.get(currentIndex))
                    //}

                    function findIndexFor(name)
                    {
                        var c = findIndex(model, function(item) {
                            return item.name === name
                        });
                        return c;
                    }

                    Component.onCompleted: function()
                    {
                        /// search in the model if there is one item with the
                        /// same name as in the combobox.
                        if (findIndexFor(root.currentContentName))
                            currentIndex = findIndexFor(root.currentContentName);
                    }
                }

                /// Open a new windows with this content.
                Button
                {
                    position: cornerPositions["Single"]
                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/icon/subWindow.png"
                    }
                    onClicked: {
                        windowComponent.createObject(SofaApplication, {"source": "file:///"+listModel.get(comboBox.currentIndex).filePath,
                                                         "title" : comboBox.currentText });
                    }

                    Component {
                        id: windowComponent

                        Window {
                            property url source

                            id: window
                            width: 400
                            height: 600
                            modality: Qt.NonModal
                            flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
                            visible: true
                            color: "#787878"

                            Loader {
                                id: loader
                                anchors.fill: parent
                                source: window.source
                            }
                        }
                    }
                }

                Label {
                    id: showAllLabel
                    Layout.alignment: Qt.AlignVCenter
                    text: "Live Coding:"
                    color: "black"
                }
                CheckBox {
                    id : checkBoxEditting
                    Layout.alignment: Qt.AlignVCenter
                    ToolTip {
                        text: "Live Coding"
                        description: "Activates Live coding on the current view"
                    }
                }
            }
        }

        /// This is the content of the view.
        Item {
            id: loaderLocation
            anchors.top : toolBar.bottom
            anchors.left : parent.left
            anchors.right : parent.right
            anchors.bottom : parent.bottom
            visible: contentItem
            Rectangle {
                anchors.fill: parent
                color: SofaApplication.style.contentBackgroundColor
            }

            property Item contentItem
            property string errorMessage

            /// @brief refresh the content "area" of the view with the given name
            function refresh(viewEntry)
            {
                if(!viewEntry)
                    return

                var name = viewEntry.name;
                var source = viewEntry.filePath;

                /// Check if there is an existing view loaded
                /// If this is the case the view should be destroyed and the content set
                /// to null.
                if(loaderLocation.contentItem) {
                    loaderLocation.contentItem.destroy();
                    loaderLocation.contentItem = null;
                }


                /// Load the component from a qml file.
                var contentComponent = Qt.createComponent("file://"+source);
                if(contentComponent.status === Component.Error)
                {
                    ///TODO(dmarchal 28/01/2019) Fix loader.
                    console.error("Error while loading: " + contentComponent.errorString());
                    loaderLocation.contentItem = Qt.createComponent("qrc:/SofaBasics/DynamicContent_Error.qml").createObject(loaderLocation.contentItem);
                    return;
                }

                /// Create an uid in the SofaApplication settings.
                if(root.contentUiId === 0)
                {
                    root.contentUiId = SofaApplication.uiSettings.generate();
                }

                ///TODO(dmarchal 28/01/2019) I don't understand what is this for.
                var contentProperties = root.properties;
                if(!contentProperties)
                    contentProperties = "";

                contentProperties["anchors.fill"] = loaderLocation;
                var content = contentComponent.createObject(loaderLocation, contentProperties);

                loaderLocation.contentItem = content;
                root.currentContentName = name;
            }
        }
    }
}


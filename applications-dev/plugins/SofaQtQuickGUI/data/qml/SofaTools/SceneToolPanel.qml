import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import SofaBasics 1.0

CollapsibleGroupBox {
    id: root

    visible: interfaceToolPanel ? Loader.Error === interfaceToolPanel.status || Loader.Ready === interfaceToolPanel.status : false
    title: "Scene ToolPanel"

    property int priority: 95
    property var scene: null

    readonly property Loader interfaceLoader: scene ? scene.interfaceLoader : null
    readonly property QtObject interfaceObject: interfaceLoader ? interfaceLoader.item : null
    readonly property Component interfaceToolPanel: interfaceObject ? interfaceObject.toolpanel : null

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Loader {
            id: loader
            Layout.fillWidth: true
            Layout.preferredHeight: item ? item.implicitHeight : 0
            asynchronous: false
            sourceComponent: interfaceToolPanel
        }

        BusyIndicator {
            visible: running
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            running: Loader.Loading === loader.status
        }

        Text {
            visible: Loader.Error === root.interfaceLoader.status || Loader.Error === loader.status
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
            textFormat: Text.RichText

            text: "<b><font color='darkred'>Error loading scene toolpanel, take a look in the console for error log</font></b>"
        }
    }

/*    readonly property alias status: d.status
    readonly property alias item: d.item

    QtObject {
        id: d

        property Item item
        property int status: Loader.Null

        property Timer timer: Timer {
            running: false
            repeat: false
            interval: 1

            onTriggered: {
                errorLabel.text = "";

                // use a fresh version of the gui if it's a reload by removing the old version of the cache
                SofaToolsScript.Tools.trimCache();

                if(0 !== root.source.toString().length) {
                    var componentFactory = Qt.createComponent(root.source);
                    if(Component.Ready === componentFactory.status)
                        d.item = componentFactory.createObject(layout, {"Layout.fillWidth": true});

                    if(!d.item) {
                        errorLabel.text = "Cannot create Component from:" + root.source + "\n\n";
                        errorLabel.text += componentFactory.errorString().replace("\n", "\n\n");
                        d.status = Loader.Error;
                        return;
                    }

                    d.status = Loader.Ready;
                }
            }
        }
    }

    onSourceChanged: {
        if(d.item)
            d.item.destroy();

        if(0 !== root.source.toString().length)
            d.status = Loader.Loading;
        else
            d.status = Loader.Null;

        // delay loading of the component to the next frame to let qml completely destroy the previous one allowing us to trim it from cache
        d.timer.start();
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        spacing: 0

//        Item {
//            Layout.fillWidth: true
//            implicitHeight: busyIndicator.implicitHeight
//            visible: busyIndicator.running

//            BusyIndicator {
//                id: busyIndicator
//                anchors.horizontalCenter: parent.horizontalCenter
//                anchors.top: parent.top
//                implicitWidth: 100
//                implicitHeight: implicitWidth
//                running: root.item ? false : Loader.Loading === root.status
//            }
//        }

        GroupBox {
            Layout.fillWidth: true
            implicitWidth: 0
            visible: Loader.Error === status
            title: "Error Loading Scene GUI"

            ColumnLayout {
                anchors.fill: parent

                TextArea {
                    id: errorLabel
                    Layout.fillWidth: true
                    Layout.preferredHeight: 250
                    wrapMode: Text.WordWrap
                    readOnly: true
                }
            }
        }
    }
*/
}

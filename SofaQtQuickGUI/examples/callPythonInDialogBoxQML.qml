import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0

SofaSceneInterface {
    id: root

    Window {
        id: dialog
        width: 400
        height: 200
        modality: Qt.NonModal
        flags: Qt.Dialog | Qt.WindowStaysOnTopHint
        visible: true

        ColumnLayout {
            id: layout
            anchors.fill: parent

            GroupBox {
                Layout.fillWidth: true
                Layout.fillHeight: true

                GridLayout {
                    anchors.fill: parent
                    columns: 3

                    Item {
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        Layout.preferredHeight: aboutLabel.implicitHeight

                        Label {
                            id: aboutLabel
                            anchors.fill: parent

                            wrapMode: Text.WordWrap
                            text: "<b>About ?</b><br />
                                The goal of this example is to show you how to call Python functions defined in your sofa scene to get / set data using the sofaPythonInteractor QML object.<br />"
                        }
                    }

                    Label {
                        Layout.preferredWidth: implicitWidth
                        text: ""
                    }
                    RowLayout {
                        Layout.columnSpan: 2
                        Layout.fillWidth: true

                        Label {
                            Layout.fillWidth: true
                            Layout.preferredWidth: 20
                            horizontalAlignment: Text.AlignHCenter
                            text: "X    "
                            color: enabled ? "red" : "darkgrey"
                            font.bold: true
                        }
                        Label {
                            Layout.fillWidth: true
                            Layout.preferredWidth: 20
                            horizontalAlignment: Text.AlignHCenter
                            text: "Y    "
                            color: enabled ? "green" : "darkgrey"
                            font.bold: true
                        }
                        Label {
                            Layout.fillWidth: true
                            Layout.preferredWidth: 20
                            horizontalAlignment: Text.AlignHCenter
                            text: "Z    "
                            color: enabled ? "blue" : "darkgrey"
                            font.bold: true
                        }
                    }

                    // gravity
                    Label {
                        Layout.preferredWidth: implicitWidth
                        text: "Gravity"
                    }
                    Vector3DSpinBox {
                        id: gravity
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        decimals: 4
                        stepSize:0.01

                        function update() {

                            // PythonScriptController given by NAME
                            // slower method but more generic
                            // the component can be moved
                            // or even be removed (this will generate a warning but no crash)

                            sofaScene.sofaPythonInteractor.call("script", "setGravity", vx, vy, vz);
                        }

                        Component.onCompleted: {

                            // PythonScriptController given by PATH
                            // faster method for "static" component

                            setValueFromArray(sofaScene.sofaPythonInteractor.call("/script", "getGravity"));

                            onVxChanged.connect(update);
                            onVyChanged.connect(update);
                            onVzChanged.connect(update);
                        }

                        Connections {
                            target: sofaScene

                            // PythonScriptController given by PATH
                            // faster method for "static" component

                            onStepEnd: gravity.setValueFromArray(sofaScene.sofaPythonInteractor.call("/script", "getGravity"));
                        }
                    }

                    // point location
                    Label {
                        Layout.preferredWidth: implicitWidth
                        text: "Position"
                    }
                    Vector3DSpinBox {
                        id: pointLocation
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        decimals: 4
                        stepSize:0.01
                        enabled: false

                        Connections {
                            target: sofaScene
                            onStepEnd: if(sofaScene.ready) pointLocation.update()
                            onReseted: if(sofaScene.ready) pointLocation.update()
                            Component.onCompleted: if(sofaScene.ready) pointLocation.update()
                        }

                        function update() {
                            setValueFromArray(sofaScene.sofaPythonInteractor.call("/script", "getPointLocation"));
                        }
                    }
                }
            }
        }
    }
}

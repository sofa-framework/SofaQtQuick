import QtQuick 2.0
import QtQml.Models 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0

SofaSceneInterface {
    id: root

    toolpanel: ColumnLayout {
        enabled: sofaScene.ready
        anchors.fill: parent

        property var sliderNum: 0

        GroupBox {
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: about.implicitHeight

                    Label {
                        id: about
                        anchors.fill: parent

                        wrapMode: Text.WordWrap
                        text: "<b>About ?</b><br />
                            The goal of this example is to show you how to create a dynamic GUI.<br />"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: "Sliders number:"
                    }

                    SpinBox {
                        Layout.fillWidth: true
                        minimumValue: 0
                        //maximumValue: 200
                        value: 0

                        onValueChanged: {
                            var diff = value - listModel.count;
                            if(diff < 0) {
                                listModel.remove(listModel.count + diff, -diff);
                            } else if(diff > 0) {
                                for(var i = 0; i < diff; ++i)
                                    listModel.append({index: listModel.count});
                            }
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ListView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: contentItem.height

                        model: VisualDataModel {
                            model: ListModel {
                                id: listModel
                            }

                            delegate: RowLayout {
                                id: delegateItem
                                anchors.left: parent.left
                                anchors.right: parent.right
                                spacing: 5

                                Text {
                                    Layout.preferredWidth: 100
                                    Layout.preferredHeight: implicitHeight
                                    text: "Index: " + index
                                    font.bold: true
                                }
                                Slider {
                                    id: slider
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: slider.implicitHeight
                                    minimumValue: 0.0
                                    maximumValue: 1.0
                                    value: 0.0
                                    stepSize: 0.01

                                    onValueChanged: sofaScene.sofaPythonInteractor.call("PythonScript", "setValueAtIndex", index, value)
                                }
                                TextField {
                                    Layout.preferredWidth: 100
                                    Layout.preferredHeight: implicitHeight
                                    text: slider.value.toFixed(2)
                                    enabled: false
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

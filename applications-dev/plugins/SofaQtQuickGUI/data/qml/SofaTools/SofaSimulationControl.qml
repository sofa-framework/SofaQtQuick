/*
Copyright 2015, Anatoscope

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
*/

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0

CollapsibleGroupBox {
    id: root
    implicitWidth: 0

    title: "Simulation Control"
    property int priority: 100

    property var sofaScene

    enabled: sofaScene ? sofaScene.ready : false

    GridLayout {
        anchors.fill: parent
        columns: 3

        Label {
            Layout.preferredWidth: implicitWidth
            text: "DT"
        }
        SpinBox {
            id: dtSpinBox
            Layout.columnSpan: 2
            Layout.fillWidth: true
            decimals: 3
            prefix: value <= 0 ? "Real-time " : ""
            suffix: " seconds"
            stepSize: 0.001
            value: sofaScene ? sofaScene.dt : 0.04
            onValueChanged: if(sofaScene) sofaScene.dt = value

            Component.onCompleted: {
                valueChanged();
            }
        }

        Label {
            Layout.preferredWidth: implicitWidth
            text: "Interaction stiffness"
        }
        Slider {
            id: interactionStiffnessSlider
            Layout.fillWidth: true
            maximumValue: 1000
            value: sofaScene ? sofaScene.sofaParticleInteractor.stiffness : 100
            onValueChanged: if(sofaScene) sofaScene.sofaParticleInteractor.stiffness = value
            stepSize: 1

            Component.onCompleted: {
                minimumValue = 1;
            }
        }
        TextField {
            Layout.preferredWidth: 40
            enabled: false
            text: interactionStiffnessSlider.value
        }

        Button {
            id: displayGraphButton
            Layout.columnSpan: 3
            Layout.fillWidth: true
            text: "Display SofaScene graph"
            tooltip: ""
            onClicked: {
                displayGraphText.text = sofaScene ? sofaScene.dumpGraph() : ""
                displayGraphDialog.open();
            }

            Dialog {
                id: displayGraphDialog
                title: "Simulation SofaScene Graph"
                width: 800
                height: 600

                contentItem: Item {
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 5
                        TextArea {
                            id: displayGraphText
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            readOnly: true

                            Component.onCompleted: {
                                wrapMode = TextEdit.NoWrap;
                            }
                        }
                        Button {
                            Layout.fillWidth: true
                            text: "Hide"
                            onClicked: displayGraphDialog.close()
                        }
                    }
                }
            }
        }
    }
}

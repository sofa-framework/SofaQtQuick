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
import SofaApplication 1.0

ColumnLayout {
    id: root
    spacing: 0
    enabled: sofaScene ? sofaScene.ready : false

    width: 300
    height: gridLayout.implicitHeight

    property var sofaScene: SofaApplication.sofaScene

    ScrollView {
        id: scrollView
        Layout.fillWidth: true
        Layout.preferredHeight: Math.min(root.height, gridLayout.implicitHeight)

        GridLayout {
            id: gridLayout
            width: scrollView.width - 9
            columns: 3

            Label {
                Layout.preferredWidth: implicitWidth
                text: "DT (s)"
            }
            TextField {
                id: dtSpinBox
                Layout.columnSpan: 2
                Layout.fillWidth: true
                validator: DoubleValidator {bottom: 0}
                text: root.sofaScene ? root.sofaScene.dt.toString() : Number(0.04).toString()

                onAccepted: {
                    if(root.sofaScene) root.sofaScene.dt = Number(text);
                    if(0 === Number(text))
                        text = "Real-time";
                }
            }

            Label {
                Layout.preferredWidth: implicitWidth
                text: "Interaction stiffness"
                visible : sofaScene && sofaScene.sofaParticleInteractor
            }

            // TODO this should depend on the selected particle-interactor so every options can be set
            Slider {
                id: interactionStiffnessSlider
                Layout.fillWidth: true
                maximumValue: 1e4
                value: ( sofaScene && sofaScene.sofaParticleInteractor ) ? sofaScene.sofaParticleInteractor.stiffness : 0
                onValueChanged: if(sofaScene && sofaScene.sofaParticleInteractor ) sofaScene.sofaParticleInteractor.stiffness = value
                stepSize: 1
                enabled: sofaScene && sofaScene.sofaParticleInteractor
                visible : sofaScene && sofaScene.sofaParticleInteractor

                Component.onCompleted: {
                    minimumValue = 1;
                }
            }
            TextField {
                Layout.preferredWidth: 40
                enabled: false
                text: interactionStiffnessSlider.value
                visible : sofaScene && sofaScene.sofaParticleInteractor
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

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

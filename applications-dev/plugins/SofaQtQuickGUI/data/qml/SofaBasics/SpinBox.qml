/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - initial version from Anatoscope
    - damien.marchal@univ-lille.fr
    - bruno.josue.marques@inria.fr
********************************************************************/

import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0
import SofaBasics 1.0 as SB

Rectangle {
    id: control

    property bool enabled: true  // like in other QtQuick controls, enables / disables the edition of this control
    property bool showIndicators: true  // shows / hides the up / down indicators for this spinBox

    property string prefix: ""  // a prefix for this spinbox (the name of the variable for instance "x: ")
    property string suffix: ""  // a suffix for this spinbox (for instance a unit of measure)
    property int decimals: 2  // the number of decimals (0 for integers)
    property double step: 0.1  // the step size to scale mouse / indicators interactions
    property double value: 50.0  // the value stored in this spinbox

    property alias cornerPositions: backgroundID.cornerPositions
    property alias position: backgroundID.position

    implicitHeight: 20
    implicitWidth: content.implicitWidth + upIndicator.width + downIndicator.width

    property bool isEditing: false
    onIsEditingChanged: {
        if (isEditing) {
            var w = modeLoader.item.implicitWidth
            modeLoader.sourceComponent = textFieldMode
            modeLoader.item.implicitWidth = w // readjust width after loading the textField
        }
        else {
            modeLoader.sourceComponent = spinBoxMode
        }
    }
    color: "transparent"
    function formatText(v, prefix, suffix) {
        var str = ""
        if (prefix !== "")
            str += qsTr(prefix)
        str += qsTr("%1").arg(v.toFixed(decimals))
        if (suffix !== "")
            str += qsTr(suffix)
        return str
    }
    function setValue(initialValue, incrVal)
    {
        var step = control.step
        var newValue = initialValue + incrVal
        control.value = newValue
    }


    Rectangle {
        id: upIndicator
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: control.right
        width: 20
        implicitWidth: 20
        height: parent.height
        visible: !isEditing && showIndicators
        color: "transparent"
        Image {
            id: rightup
            x: control.mirrored ? 2 : parent.width - width - 2
            y: parent.height / 4
            width: 9
            height: width
            source: "qrc:icon/spinboxRight.png"
        }

        MouseArea {
            id: upMouseArea
            anchors.fill: upIndicator
            hoverEnabled: true
            acceptedButtons: control.enabled ? Qt.LeftButton : Qt.NoButton

            onHoveredChanged: {
                cursorShape = Qt.ArrowCursor
                if (containsMouse) {
                    backgroundID.setControlState(control.enabled, containsMouse, upIndicator.focus)
                } else {
                    backgroundID.setControlState(control.enabled, containsMouse, upIndicator.focus)
                }
            }
            onClicked: {
                control.setValue(control.value, control.step)
            }
        }
    }

    Rectangle {
        id: downIndicator
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        width: 20
        implicitWidth: 20
        height: parent.height
        color: "transparent"
        visible: !isEditing && showIndicators
        Image {
            id: leftdown
            x: control.mirrored ? parent.width - width - 2 : 2
            y: parent.height / 4
            width: 9
            height: width
            source: "qrc:icon/spinboxLeft.png"
        }
        MouseArea {
            id: downMouseArea
            anchors.fill: downIndicator
            hoverEnabled: true
            acceptedButtons: control.enabled ? Qt.LeftButton : Qt.NoButton
            onHoveredChanged: {
                cursorShape = Qt.ArrowCursor
                if (containsMouse) {
                    backgroundID.setControlState(control.enabled, containsMouse, upIndicator.focus)
                } else {
                    backgroundID.setControlState(control.enabled, containsMouse, upIndicator.focus)
                }
            }

            onClicked: {
                control.setValue(control.value, -control.step)
            }
        }
    }

    Rectangle {
        id: content
        anchors.left: isEditing || !showIndicators ? parent.left : downIndicator.right
        anchors.right: isEditing || !showIndicators ? parent.right: upIndicator.left
        anchors.verticalCenter: parent.verticalCenter
        color: "transparent"
        implicitWidth: modeLoader.item.implicitWidth
        implicitHeight: 20

        Component {
            id: spinBoxMode
            Label {
                id: labelID
                anchors.centerIn: parent
                text: formatText(value, prefix, suffix)
                color: control.enabled ? "black" : "#464646"
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                MouseArea {
                    id: contentMouseArea

                    property var initialPosition : Qt.vector2d(0,0)
                    property var initialValue

                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: control.enabled ? Qt.LeftButton : Qt.NoButton

                    onHoveredChanged: {
                        if (control.enabled) {
                            cursorShape = Qt.SizeHorCursor
                        }
                        if (containsMouse) {
                            backgroundID.setControlState(control.enabled, containsMouse, upIndicator.focus)
                        } else {
                            backgroundID.setControlState(control.enabled, containsMouse, upIndicator.focus)
                        }
                    }

                    onPressed:
                    {
                        initialValue = control.value
                        initialPosition = Qt.vector2d(mouseX, mouseY)
                    }

                    onPositionChanged:
                    {
                        if (!pressed)
                            return
                        var currentPosition = Qt.vector2d(mouseX, mouseY)
                        setValue(initialValue, (currentPosition.x - initialPosition.x) * 0.1)
                    }

                    onReleased: {
                        var currentPosition = Qt.vector2d(mouseX, mouseY)
                        if (currentPosition.x - initialPosition.x === 0)
                            isEditing = true
                    }
                }
            }

        }

        Component {
            id: textFieldMode
            SB.TextField {
                id: textFieldID

                anchors.centerIn: parent
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter

                text: formatText(value, prefix, suffix).replace(prefix, "").replace(suffix, "")
                validator: DoubleValidator{}
                color: control.enabled ? "black" : "#464646"
                focus: true
                selectByMouse: true
                onEditingFinished: {
                    value = Number(Number(text.replace(/[^\d.-]/g, '')).toFixed(decimals)).toString();
                    isEditing = false
                }

                Component.onCompleted: {
                    forceActiveFocus()
                    selectAll()                    
                }
            }
        }

        Loader {
            id: modeLoader
            sourceComponent: spinBoxMode
            anchors.fill: parent
        }

    }

    onEnabledChanged: {
        backgroundID.setControlState(control.enabled, false, control.focus)
    }
    Component.onCompleted: {
        backgroundID.setControlState(control.enabled, false, control.focus)
    }

    ControlsBackground {
        id: backgroundID
        z: -1
        implicitWidth: parent.width
        implicitHeight: parent.implicitHeight

        borderColor: control.enabled ? "#393939" : "#505050";
        controlType: controlTypes["InputField"]
    }
}

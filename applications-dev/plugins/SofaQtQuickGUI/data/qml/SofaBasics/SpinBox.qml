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

import QtQuick 2.4
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0
import SofaBasics 1.0 as SB

Rectangle {
    id: control

    property bool readOnly: false // like in other QtQuick controls, enables / disables edition for this control
    property bool enabled: true  // like in other QtQuick controls, enables / disables the edition of this control & grays it out
    property bool showIndicators: true  // shows / hides the up / down indicators for this spinBox

    property string prefix: ""  // a prefix for this spinbox (the name of the variable for instance "x: ")
    property string suffix: ""  // a suffix for this spinbox (for instance a unit of measure)
    property int precision: 6  // visual round up of the
    property double step: 0.01  // the step size to scale mouse / indicators interactions
    property double value: 50.0  // the value stored in this spinbox

    property double from: -Infinity
    property double to: Infinity

    property alias cornerPositions: backgroundID.cornerPositions
    property alias position: backgroundID.position

    /// A signal to set the value ?
    signal valueEditted (double newvalue)

    implicitHeight: 20
    implicitWidth: content.implicitWidth + upIndicator.width + downIndicator.width
    activeFocusOnTab: true
    onActiveFocusChanged: isEditing = true

    property bool isEditing: false
    onIsEditingChanged: {
        if (isEditing) {
            var w = modeLoader.item.width
            modeLoader.sourceComponent = textFieldMode
            modeLoader.item.width = w // readjust width after loading the textField
        }
        else {
            modeLoader.sourceComponent = spinBoxMode
        }
    }
    color: "transparent"


    TextMetrics {
        id: textMetrics
        font.family: "Arial"
        elide: Text.ElideRight
        elideWidth: control.width - 10
        text: control && control.value ?  "XXX"+formatText(control.value, control.prefix, control.suffix) : ""
    }


    function formatText(v, prefix, suffix) {
        var str = ""
        if (prefix !== "")
            str += qsTr(prefix)
        str += Number(v.toPrecision((!precision) ? 6 : precision)).toString()
        if (suffix !== "")
            str += qsTr(suffix) + " "
        return str
    }

    function incValue(initialValue, incrVal)
    {
        var step = control.step
        var newValue = initialValue + incrVal
        if (newValue > to)
            newValue = to
        if (newValue < from)
            newValue = from
        control.value = newValue
        valueEditted(value)
    }

    onValueChanged:
    {
        textMetrics.text = formatText(value, control.prefix, control.suffix)
    }

    Rectangle {
        id: upIndicator
        anchors.verticalCenter: control.verticalCenter
        anchors.right: control.right
        width: 20
        implicitWidth: 20
        height: control.height
        visible: !isEditing && showIndicators
        color: "transparent"
        Image {
            id: rightup
            x: control.mirrored ? 2 : upIndicator.width - width - 2
            y: upIndicator.height / 4
            width: 9
            height: width
            source: "qrc:icon/spinboxRight.png"
        }

        MouseArea {
            id: upMouseArea
            anchors.fill: upIndicator
            hoverEnabled: true
            acceptedButtons: !control.readOnly && control.enabled ? Qt.LeftButton : Qt.NoButton

            onHoveredChanged: {
                cursorShape = Qt.ArrowCursor
                backgroundID.setControlState(control.enabled, containsMouse || control.readOnly, upIndicator.focus)
            }
            onClicked: {
                control.incValue(control.value, control.step)
            }
        }
    }

    Rectangle {
        id: downIndicator
        anchors.verticalCenter: control.verticalCenter
        anchors.left: control.left
        width: 20
        implicitWidth: 20
        height: control.height
        color: "transparent"
        visible: !isEditing && showIndicators
        Image {
            id: leftdown
            x: control.mirrored ? downIndicator.width - width - 2 : 2
            y: downIndicator.height / 4
            width: 9
            height: width
            source: "qrc:icon/spinboxLeft.png"
        }
        MouseArea {
            id: downMouseArea
            anchors.fill: downIndicator
            hoverEnabled: true
            acceptedButtons: !control.readOnly && control.enabled ? Qt.LeftButton : Qt.NoButton

            onHoveredChanged: {
                cursorShape = Qt.ArrowCursor
                backgroundID.setControlState(control.enabled, containsMouse || control.readOnly, upIndicator.focus)
            }


            onClicked: {
                control.incValue(control.value, -control.step)
            }
        }
    }

    Rectangle {
        id: content
        anchors.left: isEditing || !showIndicators ? control.left : downIndicator.right
        anchors.right: isEditing || !showIndicators ? control.right: upIndicator.left
        anchors.verticalCenter: control.verticalCenter
        color: "transparent"
        implicitHeight: 20


        Component {
            id: spinBoxMode
            TextInput {
                id: labelID

                anchors.centerIn: parent
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter

                text: textMetrics.elidedText

                color: control.enabled ? (control.readOnly ? "#393939" : "black") : "#464646"
                clip: true
                selectByMouse: control.readOnly || control.enabled ? true : false
                readOnly: true
                MouseArea {
                    id: contentMouseArea

                    property var initialPosition : Qt.vector2d(0,0)
                    property var initialValue

                    anchors.fill: labelID
                    hoverEnabled: true
                    preventStealing: true
                    acceptedButtons: !control.readOnly && control.enabled ? Qt.LeftButton : Qt.NoButton

                    onHoveredChanged: {
                        if (control.enabled && !control.readOnly) {
                            cursorShape = Qt.SizeHorCursor
                        }
                        backgroundID.setControlState(control.enabled, containsMouse || control.readOnly, upIndicator.focus)
                    }

                    onPressed:
                    {
                        initialValue = control.value
                        initialPosition = Qt.vector2d(mouseX, mouseY)
                        // Ugly, but works: removes the activeFocus from potential other activeFocused spinBoxes
                        forceActiveFocus()
                        focus = false
                        focus = true
                    }

                    onPositionChanged:
                    {
                        if (!pressed)
                            return
                        var currentPosition = Qt.vector2d(mouseX, mouseY)
                        incValue(initialValue, (currentPosition.x - initialPosition.x) * step)
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

//                text: formatText(value, prefix, suffix, false).replace(prefix, "").replace(suffix, "")
                text: +(Number(control.value).toString())
                clip: true
                validator: DoubleValidator{}
                color: control.readOnly ? "#393939" : "black"
                focus: true
                selectByMouse: true
                onEditingFinished: {
                    var v = +text
                    if (v > to)
                        v = to
                    if (v < from)
                        v = from
                    value=v
                    valueEditted(v)
                    isEditing = false
                }

                Component.onCompleted: {
                    forceActiveFocus()
                    selectAll()
                }
                position: backgroundID.position
                enabled: control.enabled
                readOnly: control.readOnly

            }
        }

        Loader {
            id: modeLoader
            sourceComponent: spinBoxMode
            anchors.fill: parent
        }
    }

    onEnabledChanged: {
        backgroundID.setControlState(control.enabled, control.readOnly ? true : false, control.focus)
    }
    onReadOnlyChanged: {
        backgroundID.setControlState(control.enabled, control.readOnly ? true : false, control.focus)
    }
    Component.onCompleted: {
        backgroundID.setControlState(control.enabled, control.readOnly ? true : false, control.focus)
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

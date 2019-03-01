import QtQuick 2.0
import QtQuick.Controls 2.4
import SofaApplication 1.0
import SofaColorScheme 1.0

Rectangle {
    id: gridID
    // Sadly, enums don't work in QML.... instead we use a dict... In the future we should replace that for a bound C++ enum
    property var cornerPositions: ({
                                       "Single": 0,
                                       "TopLeft": 1,
                                       "Top": 2,
                                       "TopRight" : 3,
                                       "Left" : 4,
                                       "Middle": 5,
                                       "Right": 6,
                                       "BottomLeft": 7,
                                       "Bottom": 8,
                                       "BottomRight": 9
                                   })
    property var controlTypes: ({
                                    "Button": 0,
                                    "ComboBox": 1,
                                    "CheckBox": 2,
                                    "InputField": 3,
                                    "ToggleButton": 4
                                })

    property int controlType: controlTypes["Button"]
    property int position: cornerPositions["Single"]
    property string borderColor: "#595959"
    property int cornerRadius: 4
    property string backgroundColor: SofaApplication.style.contentBackgroundColor
    property bool noGradient: false

    ColorScheme { id: colorScheme }
    property Item currentcontrolType: (controlType === controlTypes["Button"]) ? colorScheme.button : (controlType === controlTypes["ComboBox"]) ? colorScheme.comboBox : (controlType === controlTypes["CheckBox"]) ? colorScheme.checkBox : (controlType === controlTypes["ToggleButton"]) ? colorScheme.toggleButton : colorScheme.inputField
    property Item currentGradients: currentcontrolType.enabled

    implicitWidth: (controlType === controlTypes["CheckBox"]) ? 16 : 20
    implicitHeight: (controlType === controlTypes["CheckBox"]) ? 16 : 20
    color: "transparent"
    border.color: "transparent"

    function setControlState(enabled, hovered, pressed)
    {
        if (!enabled)
        {
            if (hovered && gridID.currentcontrolType.disabledHover)
                currentGradients = gridID.currentcontrolType.disabledHover
            else
                currentGradients = gridID.currentcontrolType.disabled
        } else {
            if (pressed)
            {
                if (hovered && gridID.currentcontrolType.pressedHover)
                    currentGradients = gridID.currentcontrolType.pressedHover
                else
                    currentGradients = gridID.currentcontrolType.pressed
            }
            else if (hovered)
                currentGradients = gridID.currentcontrolType.enabledHover
            else
                currentGradients = gridID.currentcontrolType.enabled
        }
    }
    Rectangle {
        z: 2
        id: topLeft
        width: gridID.width / 2
        height: gridID.height / 2
        border.color: borderColor
        gradient: noGradient ? null : gridID.currentGradients.topGradient
        radius: (gridID.position === 0 || gridID.position === 1 || gridID.position === 2 || gridID.position === 4) ? cornerRadius : 0
        color: gridID.backgroundColor
    }
    Rectangle {
        z: 2
        id: topRight
        width: gridID.width / 2
        height: gridID.height / 2
        border.color: borderColor
        anchors.left: topLeft.right
        gradient: noGradient ? null : gridID.currentGradients.topGradient
        radius: (gridID.position === 0 || gridID.position === 2 || gridID.position === 3 || gridID.position === 6) ? cornerRadius : 0
        color: gridID.backgroundColor
    }
    Rectangle {
        z: 3
        id: horizontalBorder
        width: gridID.width / 3
        height: gridID.height
        border.color: borderColor

        anchors.left: topLeft.right
        anchors.leftMargin: -(gridID.width / 6)
        color: gridID.backgroundColor
    }
    Rectangle {
        z: 3
        id: horizontalPatch
        //            color: "lightgrey"
        width: gridID.width / 3
        height: gridID.height - 2
        border.color: "transparent"
        anchors.left: topLeft.right
        anchors.leftMargin: -(gridID.width / 6)
        anchors.top: topLeft.top
        anchors.topMargin: 1
        gradient: noGradient ? null : gridID.currentGradients.totalGradient
        color: gridID.backgroundColor
    }
    Rectangle {
        z: 3
        id: verticalBorder
        width: gridID.width
        height: gridID.height / 3
        border.color: borderColor
        anchors.left: topLeft.left
        anchors.top: topLeft.bottom
        anchors.topMargin: -(gridID.height / 6)
        color: gridID.backgroundColor
    }
    Rectangle {
        z: 3
        id: verticalPatch
        width: gridID.width - 2
        height: gridID.height / 3
        border.color: "transparent"
        anchors.left: topLeft.left
        anchors.leftMargin: 1
        anchors.top: topLeft.bottom
        anchors.topMargin: -(gridID.height / 6)
        gradient: noGradient ? null : gridID.currentGradients.middleGradient
        color: gridID.backgroundColor
    }
    Rectangle {
        z: 2
        id: bottomRight
        x: gridID.width / 2
        y: gridID.height / 2
        width: gridID.width / 2
        height: gridID.height / 2
        border.color: borderColor
        gradient: noGradient ? null : gridID.currentGradients.bottomGradient
        radius: (gridID.position === 0 || gridID.position === 6 || gridID.position === 8 || gridID.position === 9) ? cornerRadius : 0
        color: gridID.backgroundColor
    }
    Rectangle {
        z: 2
        id: bottomLeft
        y: gridID.height / 2
        width: gridID.width / 2
        height: gridID.height / 2
        border.color: borderColor
        gradient: noGradient ? null : gridID.currentGradients.bottomGradient
        radius: (gridID.position === 0 || gridID.position === 4 || gridID.position === 7 || gridID.position === 8) ? cornerRadius : 0
        color: gridID.backgroundColor
    }
}


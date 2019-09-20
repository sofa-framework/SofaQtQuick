import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import SofaColorScheme 1.0
import SofaBasics 1.0

TextField
{
    id: self
    enabled: true

    selectByMouse: true

    property int decimals: 3
    property int stepSize: 0.1
    property var value

//    property int refreshCounter : 0
//    onRefreshCounterChanged:
//    {
//        self.text = Number(Number(value).toFixed(decimals)).toString();
//    }

    onAccepted:
    {
        focus = false
    }

    text: Qt.binding(function() { return value.toFixed(decimals).toString() })

    function getValue()
    {
        return Number(Number(text).toFixed(decimals))
    }

    function incrementValue(initialValue, incrVal)
    {
        value = initialValue + incrVal*stepSize*0.1
    }

    MouseArea
    {
        anchors.fill: self
        preventStealing: true
        property bool isDragging: false
        property var initialPosition : Qt.vector2d(0,0)
        property var initialValue: 0.0

        onPressed:
        {
            isDragging = false
            initialValue = self.getValue()
            initialPosition = Qt.vector2d(mouseX, mouseY)
        }

        onPositionChanged:
        {
            var currentPosition = Qt.vector2d(mouseX, mouseY)
            if( Math.abs(currentPosition.x - initialPosition.x) > 10 )
            {
                isDragging = true
                self.incrementValue(initialValue, currentPosition.x - initialPosition.x)
            }
        }

        onReleased:
        {
            if(isDragging)
            {
                isDragging = false
            }
            else
            {
                self.forceActiveFocus(Qt.MouseFocusReason)
                self.cursorPosition = self.positionAt(mouseX, mouseY)
            }
        }
    }

    property var intValidator: IntValidator {
        Component.onCompleted: {
            var min = self.properties.min;
            if(undefined !== min)
                bottom = min;
        }
    }

    property var doubleValidator: DoubleValidator {
        decimals: self.decimals
    }

    validator: decimals > 0 ? doubleValidator : intValidator

}

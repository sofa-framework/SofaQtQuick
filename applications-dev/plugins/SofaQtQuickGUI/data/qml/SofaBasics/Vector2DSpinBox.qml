import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import SofaBasics 1.0

GridLayout {
    id: root
    margins: -1

    property alias  vx:                 vxSpinBox.value
    property alias  vy:                 vySpinBox.value

    function setValueFromArray(array) {
        if(undefined === array)
            return;

        var values = [Number(array[0]), Number(array[1])];
        if(values[0] !== values[0] || values[1] !== values[1]) {
            console.error("ERROR: Value is Nan");
            return;
        }

        vx = values[0];
        vy = values[1];
    }

    property real   minimumValue:      -9999999.0
    property real   maximumValue:       9999999.0
    property int    decimals:           2
    property real   stepSize:           1
    property int    preferredWidth:     20
    property bool   fillWidth:          true

    DoubleSpinBox {
        id: vxSpinBox
        Layout.fillWidth:               root.fillWidth
        Layout.preferredWidth:          root.preferredWidth
        realFrom:                       root.minimumValue
        realTo:                         root.maximumValue
        decimals:                       root.decimals
        realValue:                      0.0
        realStepSize:                   root.stepSize

        position: cornerPositions["Left"]
    }
    DoubleSpinBox {
        id: vySpinBox
        Layout.fillWidth:               root.fillWidth
        Layout.preferredWidth:          root.preferredWidth
        realFrom:                       root.minimumValue
        realTo:                         root.maximumValue
        decimals:                       root.decimals
        realValue:                      0.0
        realStepSize:                   root.stepSize
        position: cornerPositions["Right"]
    }
}

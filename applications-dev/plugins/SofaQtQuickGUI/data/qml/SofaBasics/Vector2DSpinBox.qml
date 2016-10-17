import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

GridLayout {
    id: root

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

    SpinBox {
        id: vxSpinBox
        Layout.fillWidth:               root.fillWidth
        Layout.preferredWidth:          root.preferredWidth
        minimumValue:                   root.minimumValue
        maximumValue:                   root.maximumValue
        decimals:                       root.decimals
        value:                          0.0
        stepSize:                       root.stepSize
    }
    SpinBox {
        id: vySpinBox
        Layout.fillWidth:               root.fillWidth
        Layout.preferredWidth:          root.preferredWidth
        minimumValue:                   root.minimumValue
        maximumValue:                   root.maximumValue
        decimals:                       root.decimals
        value:                          0.0
        stepSize:                       root.stepSize
    }
}

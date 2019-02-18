import QtQuick 2.0
import QtQuick.Layouts 1.0
import SofaBasics 1.0

GridLayout {
    id: root

    property alias  vx:                 vxSpinBox.value
    property alias  vy:                 vySpinBox.value
    property alias  vz:                 vzSpinBox.value
    property alias  vw:                 vwSpinBox.value

    function setValueFromArray(array) {
        if(undefined === array)
            return;

        var values = [Number(array[0]), Number(array[1]), Number(array[2]), Number(array[4])];
        if(values[0] !== values[0] || values[1] !== values[1] || values[2] !== values[2] || values[3] !== values[3]) {
            console.error("ERROR: Value is Nan");
            return;
        }

        vx = values[0];
        vy = values[1];
        vz = values[2];
        vw = values[3];
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
        realFrom:                   root.minimumValue
        realTo:                   root.maximumValue
        decimals:                       root.decimals
        realValue:                          0.0
        realStepSize:                       root.stepSize
    }
    DoubleSpinBox {
        id: vySpinBox
        Layout.fillWidth:               root.fillWidth
        Layout.preferredWidth:          root.preferredWidth
        realFrom:                   root.minimumValue
        realTo:                   root.maximumValue
        decimals:                       root.decimals
        realValue:                          0.0
        realStepSize:                       root.stepSize
    }
    DoubleSpinBox {
        id: vzSpinBox
        Layout.fillWidth:               root.fillWidth
        Layout.preferredWidth:          root.preferredWidth
        realFrom:                   root.minimumValue
        realTo:                   root.maximumValue
        decimals:                       root.decimals
        realValue:                          0.0
        realStepSize:                       root.stepSize
    }
    DoubleSpinBox {
        id: vwSpinBox
        Layout.fillWidth:               root.fillWidth
        Layout.preferredWidth:          root.preferredWidth
        realFrom:                   root.minimumValue
        realTo:                   root.maximumValue
        decimals:                       root.decimals
        realValue:                          0.0
        realStepSize:                       root.stepSize
    }
}

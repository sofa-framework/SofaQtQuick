import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import SofaBasics 1.0
import QtQuick.Dialogs 1.2

GridLayout {

    id: root

    columns: 3

    property alias translationSpinBox: translationSpinBox
    property alias rotationSpinBox: rotationSpinBox
    property alias scaleSpinBox: scaleSpinBox
    property alias uniformScaleSpinBox: uniformScaleSpinBox

    property alias tx: translationSpinBox.vx
    property alias ty: translationSpinBox.vy
    property alias tz: translationSpinBox.vz

    property alias rx: rotationSpinBox.vx
    property alias ry: rotationSpinBox.vy
    property alias rz: rotationSpinBox.vz

    property alias sx: scaleSpinBox.vx
    property alias sy: scaleSpinBox.vy
    property alias sz: scaleSpinBox.vz
    property alias s: uniformScaleSpinBox.value

    property var transformation: [tx, ty, tz, rx, ry, rz, sx, sy, sz]

    property int decimals: 2
    property double stepSize: 0.01

    // text x y z on top of the component
    Label {
        Layout.preferredWidth: implicitWidth
        text: ""
    }
    RowLayout {
        Layout.columnSpan: 2
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true
            Layout.preferredWidth: 20
            horizontalAlignment: Text.AlignHCenter
            text: "X    "
            color: enabled ? "red" : "darkgrey"
            font.bold: true
        }
        Label {
            Layout.fillWidth: true
            Layout.preferredWidth: 20
            horizontalAlignment: Text.AlignHCenter
            text: "Y    "
            color: enabled ? "green" : "darkgrey"
            font.bold: true
        }
        Label {
            Layout.fillWidth: true
            Layout.preferredWidth: 20
            horizontalAlignment: Text.AlignHCenter
            text: "Z    "
            color: enabled ? "blue" : "darkgrey"
            font.bold: true
        }
    }

    // translation
    Label {
        Layout.preferredWidth: implicitWidth
        text: "Translation"
    }
    Vector3DSpinBox {
        id: translationSpinBox
        Layout.columnSpan: 2
        Layout.fillWidth: true
        vx: 0
        vy: 0
        vz: 0
        decimals: root.decimals
        stepSize: root.stepSize
    }

    // rotation
    Label {
        Layout.preferredWidth: implicitWidth
        text: "Rotation"
    }
    Vector3DSpinBox {
        id: rotationSpinBox
        Layout.columnSpan: 2
        Layout.fillWidth: true
        vx: 0
        vy: 0
        vz: 0
        minimumValue: -360.0
        maximumValue:  360.0
        decimals: 0
        stepSize: 1
    }

    // scale
    Label {
        text: "Scale"
        verticalAlignment: scaleSpinBox.visible ? Text.AlignTop : Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: Math.max(scaleLayout.height, 30)
    }
    ColumnLayout {

        id: scaleLayout

        Layout.fillWidth: true
        Layout.columnSpan: 2

        Vector3DSpinBox {
            id: scaleSpinBox
            vx: 1
            vy: 1
            vz: 1
            decimals: root.decimals
            stepSize: root.stepSize

            Layout.fillWidth: true
        }

        // uniform scale
        DoubleSpinBox {
            id: uniformScaleSpinBox
            realFrom: 0
            realTo: 9999999.0
            realValue: 1
            decimals: root.decimals
            realStepSize: root.stepSize
            suffix: "Uniform"

            Layout.fillWidth: true

            onValueChanged: {
                root.sx = value;
                root.sy = value;
                root.sz = value;
            }
        }
    }
}

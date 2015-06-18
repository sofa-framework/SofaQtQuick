import QtQuick 2.0
import QtQml.Models 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import SofaManipulators 1.0

CollapsibleGroupBox {
    id: root

    title: "Scene Parameters"
    enabled: scene.ready

    property var sliderNum: 0

    Component {
        id: manipulator2DComponent

        Manipulator {
            id: manipulator

            Manipulator2D_Translation {
                id: txy

                xAxis: true
                yAxis: true

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Translation {
                id: tx

                xAxis: true
                yAxis: false

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Translation {
                id: ty

                xAxis: false
                yAxis: true

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Rotation {
                id: rz

                onOrientationChanged: manipulator.orientation = orientation;
            }
        }
    }

    Component.onCompleted: {
        scene.addManipulator(manipulator2DComponent.createObject(root));
    }

    ColumnLayout {
        anchors.fill: parent

        GroupBox {
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                Label {
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight

                    wrapMode: Text.WordWrap
                    text: "<b>About ?</b><br />
                        The goal of this example is to show you how to use manipulator.<br />"
                }
            }
        }
    }
}

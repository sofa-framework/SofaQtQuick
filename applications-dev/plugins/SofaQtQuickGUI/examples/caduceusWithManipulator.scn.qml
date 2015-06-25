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

                axis: "xy"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Translation {
                id: tx

                axis: "x"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Translation {
                id: ty

                axis: "y"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator2D_Rotation {
                id: rz

                onOrientationChanged: manipulator.orientation = orientation;
            }
        }
    }

    Component {
        id: manipulator3DComponent

        Manipulator {
            id: manipulator

            Manipulator3D_Translation {
                id: txy

                axis: "xy"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: tyz

                axis: "yz"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: txz

                axis: "xz"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: tx

                axis: "x"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: ty

                axis: "y"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Translation {
                id: tz

                axis: "z"

                onPositionChanged: manipulator.position = position;
            }

            Manipulator3D_Rotation {
                id: rx

                axis: "x"

                onOrientationChanged: manipulator.orientation = orientation;
            }

            Manipulator3D_Rotation {
                id: ry

                axis: "y"

                onOrientationChanged: manipulator.orientation = orientation;
            }

            Manipulator3D_Rotation {
                id: rz

                axis: "z"

                onOrientationChanged: manipulator.orientation = orientation;
            }
        }
    }

    Component.onCompleted: {
        scene.addManipulator(manipulator2DComponent.createObject(root));
        scene.addManipulator(manipulator3DComponent.createObject(root, {position: Qt.vector3d(20.0, 20.0, 20.0)}));
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

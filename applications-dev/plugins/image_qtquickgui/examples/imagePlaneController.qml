import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import SofaApplication 1.0

SofaSceneInterface {
    id: root

    toolpanel: ColumnLayout {
        GroupBox {
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: aboutLabel.implicitHeight

                    Label {
                        id: aboutLabel
                        anchors.fill: parent

                        wrapMode: Text.WordWrap
                        text: "<b>About ?</b><br />
                            The goal of this example is to show you how to add / remove points in mechanical objects from ImageViewer image planes.<br />Select the ImageViewer and take a look at its plane data to start.<br />"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignCenter

                    Label {
                        text: "MechanicalObject:"
                    }

                    ExplicitSwitch {
                        id: explicitSwitch
                        Layout.fillWidth: true

                        checked: true

                        checkedString: "  M0  "
                        checkedColor: "red"

                        notCheckedString: "  M1  "
                        notCheckedColor: "blue"

                        onClicked: {
                            if(checked)
                                SofaApplication.sofaScene.sofaPythonInteractor.call('imageViewerScript', 'useM0')
                            else
                                SofaApplication.sofaScene.sofaPythonInteractor.call('imageViewerScript', 'useM1')
                        }
                    }
                }
            }
        }
    }
}

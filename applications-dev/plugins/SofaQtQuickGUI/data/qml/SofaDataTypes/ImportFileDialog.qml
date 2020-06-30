import QtQuick 2.2
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.0
import SofaBasics 1.0
import Sofa.Core.SofaDataFileName 1.0
import SofaApplication 1.0

Dialog {
    id: importAssetDialog
    property var externalFile

    standardButtons: StandardButton.Yes | StandardButton.No
    title: qsTr("Import asset?")
    width: 400
    height: 150
    contentItem: Rectangle {
        anchors.fill: parent
        color: SofaApplication.style.contentBackgroundColor
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            TextArea {
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: qsTr("The file you selected is located outside of the project.\nThis will break its packageability.\nWould you rather copy the asset to your project directory (assets/resources)?")
            }
            RowLayout {
                Label {
                    Layout.fillWidth: true
                    text: ""
                }
                Button {
                    id: yes
                    text: "Yes"
                    onClicked: {
                        var filename = externalFile.split("/")
                        filename = filename[filename.length-1]
                        SofaApplication.copyFile(externalFile, SofaApplication.currentProject.rootDirPath + "/assets/resources/" + filename)
                        sofaData.value = SofaApplication.currentProject.rootDirPath + "/assets/resources/" + filename
                        print("copied!")
                        importAssetDialog.close()
                    }
                }
                Button {
                    id: no
                    text: "No"
                    onClicked: {
                        importAssetDialog.close()
                    }
                }
            }
        }
    }
}

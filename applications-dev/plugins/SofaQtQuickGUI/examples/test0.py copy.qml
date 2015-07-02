import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Qt.labs.settings 1.0
import SofaBasics 1.0
import Scene 1.0


CollapsibleGroupBox {
    id: root

    title: "Scene Parameters"
    enabled: scene.ready


    ColumnLayout {
        anchors.fill: parent

       
        SpinBox {
            id: penetration
            Layout.columnSpan: 2
            Layout.fillWidth: true
            decimals: 4
            stepSize:0.01
            value:  scene.pythonInteractor.call("moveController", "getPenetration")

            onValueChanged: {
                if(scene) scene.pythonInteractor.call("moveController", "setPenetration", value);
            }


        }

        
        
        ComboBox {
            id:currentIndexList
            currentIndex: 0
            model: ListModel {
                id: cbItems
                ListElement { text: "0" }
                ListElement { text: "1" }
                ListElement { text: "2" }
                ListElement { text: "3" }
                ListElement { text: "4" }
                ListElement { text: "5" }
                ListElement { text: "6" }
                ListElement { text: "7" }
                ListElement { text: "8" }
                ListElement { text: "9" }
            }
            width: 200
            onCurrentIndexChanged: {
                if(scene) {
                    scene.pythonInteractor.call("moveController", "setCurrentIndex", currentIndexList.currentIndex);
                }
            }
        }
        
        
        Button {

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "remove"

            onClicked: {
                            if(scene) {
                                scene.pythonInteractor.call("moveController", "remove");
                            }
                    }
        }
    
    
        Button {

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "up"

            onClicked: {
                            if(scene) {
                                scene.pythonInteractor.call("moveController", "move", 1, .05);
                            }
                    }
        }
        
        Button {

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "bottom"

            onClicked: {
                            if(scene) {
                                scene.pythonInteractor.call("moveController", "move", 1, -.1);
                            }
                    }
        }

        Button {

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "right"

            onClicked: {
                            if(scene) {
                                scene.pythonInteractor.call("moveController", "move", 0, .1);
                            }
                    }
        }
        
        Button {

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "left"

            onClicked: {
                            if(scene) {
                                scene.pythonInteractor.call("moveController", "move", 0, -.1);
                            }
                    }
        }
        
        
        Button {

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "backward"

            onClicked: {
                            if(scene) {
                                scene.pythonInteractor.call("moveController", "move", 2, .1);
                            }
                    }
        }
        
        Button {

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "forward"

            onClicked: {
                            if(scene) {
                                scene.pythonInteractor.call("moveController", "move", 2, -.1);
                            }
                    }
        }


    }


}

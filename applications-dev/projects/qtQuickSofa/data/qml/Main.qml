import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.3
import SofaBasics 1.0
import SofaTools 1.0
import SofaWidgets 1.0
import "qrc:/SofaCommon/SofaSettingsScript.js" as SofaSettingsScript

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    title: Qt.application.name + " - \"" + scenePath + "\""

    property var scene: SofaApplication.scene
    property var viewers: SofaApplication.viewers
    property string scenePath: ""

    style: ApplicationWindowStyle {
        background: null
    }

    Component.onCompleted: {
        showNormal();
        //showFullScreen();

        if(Qt.application.arguments.length > 1) {
            scene.source = "file:" + Qt.application.arguments[1];
        }
        else {
            if(0 !== SofaSettingsScript.Recent.scenes.length)
                scene.source = SofaSettingsScript.Recent.mostRecent();
            else
                scene.source = "file:Demos/caduceus.scn";
        }
        scenePath = scene.source.toString().replace("///", "/").replace("file:", "");
    }

    // dialog
    property FileDialog openSofaSceneDialog: openSofaSceneDialog
    FileDialog {
        id: openSofaSceneDialog
        nameFilters: ["Scene files (*.xml *.scn *.pscn *.py *.simu *)"]
        onAccepted: {
            scene.source = fileUrl;
            scenePath = scene.source.toString().replace("///", "/").replace("file:", "");
        }
    }

    property FileDialog saveSofaSceneDialog: saveSofaSceneDialog
    FileDialog {
        id: saveSofaSceneDialog
        selectExisting: false
        nameFilters: ["Scene files (*.scn)"]
        onAccepted: {
            scene.save(fileUrl);
        }
    }

    property FileDialog saveSofaSceneScreenshotDialog: saveSofaSceneScreenshotDialog
    FileDialog {
        id: saveSofaSceneScreenshotDialog
        selectExisting: false
        nameFilters: ["Scene files (*.png *.bmp)"]
        title: "Choose the file where you want to save the screenshot"
        onAccepted: {
            scene.screenshotFilename = fileUrl;
            scene.saveScreenshotInFile();
        }
    }

    menuBar: DefaultMenuBar {
        id: menuBar
        scene: root.scene
        viewers: root.viewers
    }

    toolBar: DefaultToolBar {
        id: toolBar
        scene: root.scene
    }

    DynamicSplitView {
        id: dynamicSplitView
        anchors.fill: parent
        uiId: 1
        sourceComponent: Component {
            DynamicContent {
                defaultContentName: "Viewer"
                sourceDir: "qrc:/SofaWidgets"
                properties: {"scene": root.scene}
            }
        }
    }

    statusBar: DefaultStatusBar {
        id: statusBar
        scene: root.scene
    }
}

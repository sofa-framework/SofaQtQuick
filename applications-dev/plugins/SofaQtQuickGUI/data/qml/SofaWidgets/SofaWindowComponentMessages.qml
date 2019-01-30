import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaViews 1.0

///////////////////////////////////////////////////////////////////////////:
/// This windows contains a MessageView that is configured to show only
/// the message for a given component.
///////////////////////////////////////////////////////////////////////////:
Component {
    Window {
        property var sofaComponent: null
        id: root
        width: 600
        height: 400
        modality: Qt.NonModal
        flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint |Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint
        visible: true
        color: "lightgrey"

        title: sofaComponent ? ("Messages for component: " + sofaComponent.getName()) : "No component to visualize"

        Loader {
            id: loader
            anchors.fill: parent
            sourceComponent: MessageView {
                filterByComponent: true
                sofaSelectedComponent: sofaComponent
            }
        }
    }
}

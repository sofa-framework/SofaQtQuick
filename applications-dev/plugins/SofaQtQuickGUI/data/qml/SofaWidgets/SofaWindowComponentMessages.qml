import QtQuick 2.0
import QtQuick.Controls 2.4
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
        id: self
        width: parent.Window.window.width
        height: 400
        modality: Qt.NonModal
        visible: true
        color: "lightgrey"

        property int offX : 0
        property int offY : 0
        x: parent.Window.window.x + offX
        y: parent.Window.window.y + offY

        title: sofaComponent ? ("Messages for component: " + sofaComponent.getName()) : "No component to visualize"

        Loader {
            id: loader
            anchors.fill: parent
            sourceComponent: MessageView
            {
                filterByComponent: true
                sofaSelectedComponent: sofaComponent
            }
        }
    }
}

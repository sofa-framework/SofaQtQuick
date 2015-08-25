import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.0

QtObject {
    id: root

    Component.onCompleted: {
        // do your init actions here
    }

    Component.onDestruction: {
        // do your release actions here
    }

    property Component menu: null
    property Component toolbar: null
    property Component toolpanel: null

    // private

    default property alias data: d.data
    property QtObject d: QtObject {
        id: d
        property var data: []
    }
}

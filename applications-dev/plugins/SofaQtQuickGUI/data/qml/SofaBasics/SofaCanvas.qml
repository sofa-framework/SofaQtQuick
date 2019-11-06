import QtQuick 2.0
import QtQuick.Controls 2.5
import SofaBasics 1.0
import SofaApplication 1.0

Rectangle {
    property var self: null
    property string __componentPath
    on__ComponentPathChanged: {
        self = SofaApplication.sofaScene.get(__componentPath)
    }
    property var __x: self.getData("x")
    property var __y: self.getData("y")
    x: __x.value
    y: __y.value
}

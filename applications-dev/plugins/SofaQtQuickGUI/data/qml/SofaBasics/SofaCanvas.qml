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
    property var __x: self ? self.getData("x") : 0
    property var __y: self ? self.getData("y") : 0
    x: __x ? __x.value : 0
    y: __y ? __y.value : 0
}

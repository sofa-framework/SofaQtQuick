/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.0

Item {
    id: groupbox

    /*! \internal */
    default property alias __content: container.data

    readonly property alias contentItem: container

    implicitWidth: (!anchors.fill ? container.calcWidth() : 0)
    implicitHeight: (!anchors.fill ? container.calcHeight() : 0)

    Layout.minimumWidth: implicitWidth
    Layout.minimumHeight: implicitHeight

    Accessible.role: Accessible.Grouping
    Accessible.name: title

    activeFocusOnTab: false

    data: [
        Item {
            id: container
            objectName: "container"
            z: 1
            focus: true
            anchors.fill: parent

            property Item layoutItem: container.children.length === 1 ? container.children[0] : null
            function calcWidth () { return (layoutItem ? (layoutItem.implicitWidth) +
                                                         (layoutItem.anchors.fill ? layoutItem.anchors.leftMargin +
                                                                                    layoutItem.anchors.rightMargin : 0) : container.childrenRect.width) }
            function calcHeight () { return (layoutItem ? (layoutItem.implicitHeight) +
                                                          (layoutItem.anchors.fill ? layoutItem.anchors.topMargin +
                                                                                     layoutItem.anchors.bottomMargin : 0) : container.childrenRect.height) }
        }]
}

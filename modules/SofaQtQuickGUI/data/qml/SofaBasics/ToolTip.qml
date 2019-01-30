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

import QtQuick 2.0
import QtQuick.Controls 1.0
import SofaApplication 1.0

MouseArea {
    id: root
    anchors.fill: parent
    hoverEnabled:true

    property string title: ""
    property string description: ""
    property int delay: 500
    property int xOffset: 10
    property int yOffset: 0

    onPressed: {
        d.release();
        mouse.accepted = false;
    }

    onWheel: {
        d.release();
        wheel.accepted = false;
    }

    onEntered: d.init();
    onExited: d.release();

    Component.onDestruction: d.release();

    Timer {
        id: timer
        running: false
        repeat: false
        interval: root.delay
        onTriggered: {
            var parentItem = root;
            var pos = Qt.point(mouseX + root.xOffset, mouseY + root.yOffset);

            var window = SofaApplication.itemWindow(root);
            if(window) {
                if(window.statusBar && !window.statusBar.clip)
                    parentItem = window.statusBar;
                else if(window.toolBar && !window.toolBar.clip)
                    parentItem = window.toolBar;
                else if(window.contentItem && !window.contentItem.clip)
                    parentItem = window.contentItem;
                else if(SofaApplication.isAncestor(root, window.contentItem))
                    parentItem = window.contentItem;
                else if(SofaApplication.isAncestor(root, window.toolBar))
                    parentItem = window.toolBar;
                else if(SofaApplication.isAncestor(root, window.statusBar))
                    parentItem = window.statusBar;

                if(root !== parentItem)
                    pos = root.mapToItem(parentItem, pos.x, pos.y);
            }

            var incubator = toolTipComponent.incubateObject(parentItem, {x: pos.x, y: pos.y});
            incubator.forceCompletion();

            d.toolTip = incubator.object;

            // ensure the tooltip stays in the item window
            if(window) {
                var position = SofaApplication.mapItemToScene(d.toolTip, Qt.point(0, 0));

                d.toolTip.x = d.toolTip.x + Math.max(0, Math.min(position.x, window.width - d.toolTip.width)) - position.x;
                d.toolTip.y = d.toolTip.y + Math.max(0, Math.min(position.y, window.height - d.toolTip.height)) - position.y;
            } else { // or at least in its parent item
                d.toolTip.x = Math.max(0, Math.min(d.toolTip.x, parentItem.width - d.toolTip.width));
                d.toolTip.y = Math.max(0, Math.min(d.toolTip.y, parentItem.height - d.toolTip.height));
            }
        }
    }

    Component {
        id: toolTipComponent

        Rectangle {
            implicitWidth: column.implicitWidth
            implicitHeight: column.implicitHeight

            color: "lightgoldenrodyellow"
            radius: 5
            z: 1000

            Column {
                id: column

                Text {
                    text: root.title
                    font.bold: true
                }

                Text {
                    text: root.description
                    //font.italic: true
                }
            }
        }
    }

    QtObject {
        id: d

        property Item toolTip

        function init() {
            if(0 === root.title.length && 0 === root.description.length)
                return;

            timer.start();
        }

        function release() {
            timer.stop();

            if(toolTip)
                toolTip.destroy();
        }
    }
}

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

import QtQuick 2.7

MouseArea {
    id: root
    width: 5
    height: 5
    enabled: window
    hoverEnabled: containsMouse

    property var window: null
    property real minSize: Math.min(root.width, root.height) * 4

    anchors.onLeftChanged: d.left = true;
    anchors.onRightChanged: d.right = true;
    anchors.onTopChanged: d.top = true;
    anchors.onBottomChanged: d.bottom = true;

    property point startMousePosition: Qt.point(0, 0)
    property point startPosition: Qt.point(0, 0)
    property point startSize: Qt.point(0, 0)

    cursorShape: {
        var shape = Qt.ArrowCursor;

        if(enabled) {
            if((d.left || d.right) && d.top && d.bottom)
                shape = Qt.SizeHorCursor;

            if((d.top || d.bottom) && d.left && d.right)
                shape = Qt.SizeVerCursor;

            if(d.left && d.top && !d.right && !d.bottom || !d.left && !d.top && d.right && d.bottom)
                shape = Qt.SizeFDiagCursor;

            if(d.right && d.top && !d.left&& !d.bottom || d.left && d.bottom && !d.right && !d.top)
                shape = Qt.SizeBDiagCursor;
        }

        return shape;
    }

    QtObject {
        id: d

        property bool left: false
        property bool right: false
        property bool top: false
        property bool bottom: false
    }

    function xor(a, b) {
        return (a || b) && !(a && b);
    }

    onPressed: {
        var position = mapToItem(window.contentItem, mouse.x, mouse.y);
        position = Qt.point(position.x + window.x, position.y + window.y);

        startMousePosition = position;
        startPosition = Qt.point(window.x, window.y);
        startSize = Qt.point(window.width, window.height);
    }

    onPositionChanged: {
        if(!pressed)
            return;

        var position = mapToItem(window.contentItem, mouse.x, mouse.y);
        position = Qt.point(position.x + window.x, position.y + window.y);

        var delta = Qt.point(position.x - startMousePosition.x, position.y - startMousePosition.y)

        if(d.left && !d.right)
            delta.x = -delta.x;

        if(d.top && !d.bottom)
            delta.y = -delta.y;

        var width = window.width;
        if(xor(d.left, d.right))
            width = Math.max(minSize, startSize.x + delta.x);

        var height = window.height;
        if(xor(d.top, d.bottom))
            height = Math.max(minSize, startSize.y + delta.y);

        if(d.left && !d.right)
            window.x = startPosition.x + (startSize.x - width);

        if(d.top && !d.bottom)
            window.y = startPosition.y + (startSize.y - height);

        if(Math.abs(window.width - width) >= 1.0)
            window.width = width;

        if(Math.abs(window.height - height) >= 1.0)
            window.height = height;
    }
}

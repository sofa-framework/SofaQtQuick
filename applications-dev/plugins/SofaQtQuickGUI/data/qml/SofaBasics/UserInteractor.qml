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

QtObject {
    id: root

    property string name: ""

    property bool hoverEnabled: false

    property var mouseClickedMapping: Array()
    property var mouseDoubleClickedMapping: Array()
    property var mousePressedMapping: Array()
    property var mouseReleasedMapping: Array()
    property var mouseWheelMapping: null
    property var mouseMovedMapping: null

    property var keyPressedMapping: Array()
    property var keyReleasedMapping: Array()
    property var modifierPressedMapping: Array()
    property var modifierReleasedMapping: Array()


    // mapping between user interaction and binding
    function addMouseClickedMapping(button, binding) {
        mouseClickedMapping[button] = binding;
    }

    function addMouseDoubleClickedMapping(button, binding) {
        mouseDoubleClickedMapping[button] = binding;
    }

    function addMousePressedMapping(button, binding) {
        mousePressedMapping[button] = binding;
    }

    function addMouseReleasedMapping(button, binding) {
        mouseReleasedMapping[button] = binding;
    }

    function setMouseWheelMapping(binding) {
        mouseWheelMapping = binding;
    }

    function setMouseMovedMapping(binding) {
        mouseMovedMapping = binding;
    }

    function addKeyPressedMapping(key, binding) {
        keyPressedMapping[key] = binding;
    }

    function addKeyReleasedMapping(key, binding) {
        keyReleasedMapping[key] = binding;
    }

    // event
    function mouseClicked(mouse, sofaViewer) {
        var binding = mouseClickedMapping[mouse.button];
        if(binding)
            binding(mouse, sofaViewer);
    }

    function mouseDoubleClicked(mouse, sofaViewer) {
        var binding = mouseDoubleClickedMapping[mouse.button];
        if(binding)
            binding(mouse, sofaViewer);
    }

    function mousePressed(mouse, sofaViewer) {
        var binding = mousePressedMapping[mouse.button];
        if(binding)
            binding(mouse, sofaViewer);
    }

    function mouseReleased(mouse, sofaViewer) {
        var binding = mouseReleasedMapping[mouse.button];
        if(binding)
            binding(mouse, sofaViewer);
    }

    function mouseWheel(wheel, sofaViewer) {
        var binding = mouseWheelMapping;
        if(binding)
            binding(wheel, sofaViewer);
    }

    function mouseMoved(mouse, sofaViewer) {
        var binding = mouseMovedMapping;
        if(binding)
            binding(Qt.point(Math.floor(mouse.x), Math.floor(mouse.y)), sofaViewer);
    }

    function keyPressed(event, sofaViewer) {
        var any = keyPressedMapping[Qt.Key_Any];
        if(any)
            any(event, sofaViewer);
        var keyFunc = keyPressedMapping[event.key];
        if(keyFunc)
            keyFunc(event, sofaViewer);
    }

    function keyReleased(event, sofaViewer) {
        var any = keyReleasedMapping[Qt.Key_Any];
        if(any)
            any(event, sofaViewer);
        var keyFunc = keyReleasedMapping[event.key];
        if(keyFunc)
            keyFunc(event, sofaViewer);
    }

    function init() {
        console.error("ERROR: UserInteractor_* must implement the init function !");
    }
}

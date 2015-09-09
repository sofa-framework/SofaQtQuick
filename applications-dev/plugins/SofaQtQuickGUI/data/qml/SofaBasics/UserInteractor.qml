import QtQuick 2.0
import Scene 1.0
import Viewer 1.0

QtObject {
    id: root

    property string name: ""

    property var mouseClickedMapping: Array()
    property var mouseDoubleClickedMapping: Array()
    property var mousePressedMapping: Array()
    property var mouseReleasedMapping: Array()
    property var mouseWheelMapping: null
    property var mouseMoveMapping: null

    property var keyPressedMapping: Array()
    property var keyReleasedMapping: Array()

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

    function setMouseMoveMapping(binding) {
        mouseMoveMapping = binding;
    }

    function addKeyPressedMapping(key, binding) {
        keyPressedMapping[key] = binding;
    }

    function addKeyReleasedMapping(key, binding) {
        keyReleasedMapping[key] = binding;
    }

    // event
    function mouseClicked(mouse, viewer) {
        var binding = mouseClickedMapping[mouse.button];
        if(binding)
            binding(mouse, viewer);
    }

    function mouseDoubleClicked(mouse, viewer) {
        var binding = mouseDoubleClickedMapping[mouse.button];
        if(binding)
            binding(mouse, viewer);
    }

    function mousePressed(mouse, viewer) {
        var binding = mousePressedMapping[mouse.button];
        if(binding)
            binding(mouse, viewer);
    }

    function mouseReleased(mouse, viewer) {
        var binding = mouseReleasedMapping[mouse.button];
        if(binding)
            binding(mouse, viewer);
    }

    function mouseWheel(wheel, viewer) {
        var binding = mouseWheelMapping;
        if(binding)
            binding(wheel, viewer);
    }

    function mouseMove(mouse, viewer) {
        var binding = mouseMoveMapping;
        if(binding)
            binding(mouse, viewer);
    }

    function keyPressed(event, viewer) {
        var binding = keyPressedMapping[event.key];
        if(binding)
            binding(event, viewer);
    }

    function keyReleased(event, viewer) {
        var binding = keyReleasedMapping[event.key];
        if(binding)
            binding(event, viewer);
    }

    function init() {
        console.error("UserInteractor_* must implement the init function !");
    }
}

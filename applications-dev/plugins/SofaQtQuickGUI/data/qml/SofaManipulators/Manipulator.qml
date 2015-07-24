import QtQuick 2.0
import Manipulator 1.0

Manipulator {
    id: root

    property var scene

    property list<Manipulator> manipulators
    default property alias data: root.manipulators

    onPositionChanged: {
        for(var i = 0; i < manipulators.length; ++i)
            manipulators[i].position = position;
    }

    onOrientationChanged: {
        for(var i = 0; i < manipulators.length; ++i)
            manipulators[i].orientation = orientation;
    }

    onScaleChanged: {
        for(var i = 0; i < manipulators.length; ++i)
            manipulators[i].scale = scale;
    }
}

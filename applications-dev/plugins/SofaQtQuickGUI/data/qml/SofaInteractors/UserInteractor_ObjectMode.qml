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
import SofaBasics 1.0
import SofaComponent 1.0
import SofaApplication 1.0

UserInteractor_CameraMode {
    id: root

    function init() {
        moveCamera_init();
        addMousePressedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            var selectable = sofaViewer.pickObject(Qt.point(mouse.x, mouse.y));
            if(selectable && selectable.sofaComponent) {
                    SofaApplication.selectedComponent = selectable.sofaComponent;
            }

            var selectedManipulator = SofaApplication.selectedManipulator
            if(selectedManipulator) {
                if(selectedManipulator.mousePressed)
                    selectedManipulator.mousePressed(mouse, sofaViewer);

                if(selectedManipulator.mouseMoved)
                    setMouseMovedMapping(selectedManipulator.mouseMoved);

            }
        });

        addMouseReleasedMapping(Qt.LeftButton, function(mouse, sofaViewer) {

            var selectedManipulator = SofaApplication.selectedManipulator

            if(selectedManipulator && selectedManipulator.mouseReleased)
                selectedManipulator.mouseReleased(mouse, sofaViewer);

            setMouseMovedMapping(null);
        });

    }
}

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
import SofaCompliantInteractor 1.0

UserInteractor_MoveCamera {
    id: root

    property var selectedManipulator: null
    property var selectedComponent: null

    property var interactor : SofaCompliantInteractor {
	compliance: 1e-2
    arrowSize: 0
    isCompliance : false
    color : "yellow"
    visualmodel : false
	onInteractingChanged: SofaApplication.overrideCursorShape = interacting ? Qt.BlankCursor : 0
    }
    
    function init() {

	console.log('compliant interactor init')
	
        moveCamera_init();

	addMousePressedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            selectedManipulator = sofaScene.selectedManipulator;
            selectedComponent = sofaScene.selectedComponent;
	    
            var sofaComponentParticle = sofaViewer.pickParticle(Qt.point(mouse.x, mouse.y));
	    
            if(sofaComponentParticle) {
                interactor.start(sofaComponentParticle.sofaComponent, sofaComponentParticle.particleIndex);

                setMouseMovedMapping(function(mouse, sofaViewer) {
                    var z = sofaViewer.computeDepth(interactor.interactorPosition);
                    var position = sofaViewer.mapToWorld(Qt.point(mouse.x, mouse.y), z);
                    interactor.update(position);
                });
            }
        });

        addMouseReleasedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
            if(interactor)
                interactor.release();

            if(selectedManipulator && selectedManipulator.mouseReleased)
                selectedManipulator.mouseReleased(mouse, sofaViewer);

            sofaScene.selectedManipulator = null;

            setMouseMovedMapping(null);
        });

    }
}

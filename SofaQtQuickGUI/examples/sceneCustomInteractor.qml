import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaInteractors 1.0

SofaSceneInterface {
    id: root

    property vector3d cursorWorldPosition: Qt.vector3d(0.0, 0.0, 0.0)

    Component.onCompleted: {
        SofaApplication.addInteractor("Select Red (Tag 0)", selectRedInteractor);
        SofaApplication.addInteractor("Select Blue (Tag 1)", selectBlueInteractor);
        SofaApplication.addInteractor("Particle Creator / Destructor", particleCreatorInteractor);
    }

    Component.onDestruction: {
        SofaApplication.removeInteractor(particleCreatorInteractor);
        SofaApplication.removeInteractor(selectBlueInteractor);
        SofaApplication.removeInteractor(selectRedInteractor);
    }

    // an interactor that allow us to select the red cube (because it is tagged '0' and we select the object tagged '0')
    property Component selectRedInteractor: UserInteractor {

        hoverEnabled: true

        function init() {
            addMousePressedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
                var selectable = sofaViewer.pickObjectWithTags(Qt.point(mouse.x + 0.5, mouse.y + 0.5), ['0']);
                if(selectable)
                    SofaApplication.sofaScene.selectedComponent = selectable.sofaComponent;
                else
                    SofaApplication.sofaScene.selectedComponent = null;
            });

            setMouseMovedMapping(function(mouse, sofaViewer) {
                var worldPosition = sofaViewer.projectOnGeometry(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
                root.cursorWorldPosition = Qt.vector3d(worldPosition.x, worldPosition.y, worldPosition.z).times(worldPosition.w); // will lead to NaN if the cursor is in the background
            });
        }
    }

    // an interactor that allow us to select the blue cube (because it is tagged '1' and we select the object tagged '1')
    property Component selectBlueInteractor: UserInteractor {

        hoverEnabled: true

        function init() {
            addMousePressedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
                var selectable = sofaViewer.pickObjectWithTags(Qt.point(mouse.x + 0.5, mouse.y + 0.5), ['1']);
                if(selectable)
                    SofaApplication.sofaScene.selectedComponent = selectable.sofaComponent;
                else
                    SofaApplication.sofaScene.selectedComponent = null;
            });

            setMouseMovedMapping(function(mouse, sofaViewer) {
                var worldPosition = sofaViewer.projectOnGeometry(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
                root.cursorWorldPosition = Qt.vector3d(worldPosition.x, worldPosition.y, worldPosition.z).times(worldPosition.w); // will lead to NaN if the cursor is in the background
            });
        }
    }

    // an interactor that allow us to add / remove particle in a mechanical object
    property Component particleCreatorInteractor: UserInteractor {

        hoverEnabled: true

        function init() {
            // left mouse click => add particle
            addMousePressedMapping(Qt.LeftButton, function(mouse, sofaViewer) {
                var selectable = sofaViewer.pickObject(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
                if(selectable) {
                    var selectedComponent = selectable.sofaComponent;
                    if(selectedComponent) {
                        var tags = SofaApplication.sofaScene.dataValue(selectedComponent, 'tags');
                        if(tags && 0 != tags.length) {
                            var tag = tags[0].toString();

                            SofaApplication.sofaScene.sofaPythonInteractor.call('script', 'addParticle', [selectable.position.x, selectable.position.y, selectable.position.z], tag);
                        }
                    }
                }
            });

            // right mouse click => remove particle
            addMousePressedMapping(Qt.RightButton, function(mouse, sofaViewer) {
                var selectable = sofaViewer.pickObject(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
                if(selectable) {
                    var selectedComponent = selectable.sofaComponent;
                    if(selectedComponent) {
                        var tags = SofaApplication.sofaScene.dataValue(selectedComponent, 'tags');
                        if(tags && 0 != tags.length) {
                            var tag = tags[0].toString();

                            var sofaComponentParticle = sofaViewer.pickParticle(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
                            if(sofaComponentParticle)
                                SofaApplication.sofaScene.sofaPythonInteractor.call('script', 'removeParticle', sofaComponentParticle.particleIndex, tag);
                        }
                    }
                }
            });

            setMouseMovedMapping(function(mouse, sofaViewer) {
                var worldPosition = sofaViewer.projectOnGeometry(Qt.point(mouse.x + 0.5, mouse.y + 0.5));
                root.cursorWorldPosition = Qt.vector3d(worldPosition.x, worldPosition.y, worldPosition.z).times(worldPosition.w); // will lead to NaN if the cursor is in the background
            });
        }
    }

    toolpanel: ColumnLayout {
        GroupBox {
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 3

                Item {
                    Layout.columnSpan: 3
                    Layout.fillWidth: true
                    Layout.preferredHeight: aboutLabel.implicitHeight

                    Label {
                        id: aboutLabel
                        anchors.fill: parent

                        wrapMode: Text.WordWrap
                        text: "<b>About ?</b><br />
                            The goal of this example is to show you how to attach an interactor to a scene.<br />
                            This particuliar scene has contains three interactors:<br /><br />
                            - Select Red (Tag 0): Allow you to select the red box only, the interactor pick object containing a specific tag, only referenced by the red box in this scene<br /><br />
                            - Select Blue (Tag 1): Allow you to select the blue box only, the interactor pick object containing a specific tag, only referenced by the blue box in this scene<br /><br />
                            - Particle Creator / Destructor: Allow you to add particle in a mechanical object when you left click on the red or the blue box, the right click remove a particle<br />"
                    }
                }

                Label {
                    Layout.columnSpan: 3
                    Layout.fillWidth: true
                    text: {
                        var customInteractorSelected = false;
                        if( particleCreatorInteractor === SofaApplication.focusedSofaViewer.interactorComponent ||
                                selectBlueInteractor === SofaApplication.focusedSofaViewer.interactorComponent ||
                                selectRedInteractor === SofaApplication.focusedSofaViewer.interactorComponent)
                            customInteractorSelected = true;

                        "Cursor 3D Position" + (!customInteractorSelected ? " (you must use one of the interactor above to make it work)" : "")
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    TextField {
                        id: cursorX
                        Layout.fillWidth: true

                        readOnly: true
                        enabled: !readOnly
                        validator: DoubleValidator {}
                        text: root.cursorWorldPosition.x
                    }
                }
                RowLayout {
                    Layout.fillWidth: true

                    TextField {
                        id: cursorY
                        Layout.fillWidth: true

                        readOnly: true
                        enabled: !readOnly
                        validator: DoubleValidator {}
                        text: root.cursorWorldPosition.y
                    }
                }
                RowLayout {
                    Layout.fillWidth: true

                    TextField {
                        id: cursorZ
                        Layout.fillWidth: true

                        readOnly: true
                        enabled: !readOnly
                        validator: DoubleValidator {}
                        text: root.cursorWorldPosition.z
                    }
                }
            }
        }
    }
}

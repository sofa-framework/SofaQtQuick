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
import QtQuick.Controls 2.4
import "."

Row {
    id: root
    spacing: 0

    Button {
        iconSource: "qrc:/icon/undo.png"
        enabled: MementoManager.undoReady

        onClicked: MementoManager.undoAction();

        ToolTip {
            description: MementoManager.undoActionName
        }
    }
    Button {
        iconSource: "qrc:/icon/redo.png"
        enabled: MementoManager.redoReady

        onClicked: MementoManager.redoAction();

        ToolTip {
            description: MementoManager.redoActionName
        }
    }
    Button {
        id: browseSnapshots
        iconSource: "qrc:/icon/downArrow.png"
        enabled: 0 !== MementoManager.snapshots.length && (MementoManager.undoReady || MementoManager.redoReady)

        onClicked: snapshotMenu.popup();

        ToolTip {
            description: 'Browse action history'
        }

        Menu {
            id: snapshotMenu
            title: "History"

            readonly property var currentSnapshotId: MementoManager.currentSnapshotId
            readonly property var snapshots: MementoManager.snapshots
            readonly property int neededSnapshotMenuItemNum: snapshots ? (currentSnapshotId === snapshots.length - 1 ? snapshots.length - 1 : snapshots.length) : 0

            onNeededSnapshotMenuItemNumChanged: update();

            function update() {
                // create needed menu items
                for(var i = snapshotMenu.items.length; i < neededSnapshotMenuItemNum; ++i)
                    snapshotMenu.insertItem(i, snapshotMenuItemComponent.createObject(browseSnapshots, {'index': i}));

                // delete unnecessary menu items
                for(var i = snapshotMenu.items.length - 1; i >= neededSnapshotMenuItemNum; --i)
                    snapshotMenu.removeItem(snapshotMenu.items[i]);
            }

            Component {
                id: snapshotMenuItemComponent

                MenuItem {
                    text: MementoManager.snapshotGotoMessage(snapshotId)
                    onTriggered: MementoManager.loadSnapshot(snapshotId);
                    enabled: snapshotId !== snapshotMenu.currentSnapshotId

                    property int index: -1
                    readonly property int snapshotId: snapshotMenu.neededSnapshotMenuItemNum - 1 - index
                }
            }
        }
    }
}

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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls 2.4
import SofaBasics 1.0
import Sofa.Core.SofaData 1.0
import SofaApplication 1.0
import SofaColorScheme 1.0
import SofaDataContainerListModel 1.0

ColumnLayout {
    id: root
    spacing: 0
    Layout.fillWidth: true

    function updateText()
    {
        return sofaData.value.length + (sofaData.value.length === 1 ? " Entry" : " Entries")
    }

    property SofaData sofaData
    property string numEntries : updateText()

    Connections
    {
        target: sofaData
        onValueChanged: {
            numEntries=updateText()
        }
    }

    property int refreshCounter: 0

    Component {
        id: static
        StaticContainerView {
            sofaData: root.sofaData
            Layout.fillWidth: true
        }
    }
    Component {
        id: dynamic
        DynamicContainerView {
            sofaData: root.sofaData
            Layout.fillWidth: true
        }
    }

    Loader {
        id: loader
        Layout.fillWidth: true
        Layout.minimumHeight: item.implicitHeight
        sourceComponent: root.sofaData.properties.static ? static : dynamic
    }
}

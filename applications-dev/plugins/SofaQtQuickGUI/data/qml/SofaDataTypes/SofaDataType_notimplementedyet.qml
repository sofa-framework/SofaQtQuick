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
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import SofaApplication 1.0
import SofaData 1.0

ColumnLayout {
    id: root

    property var dataObject: null

    Label {
        Layout.fillWidth: true

        text: "Data widget not implemented yet: " + (0 != root.dataObject.type.length ? root.dataObject.type : "Unknown") + " - using text edit"
        wrapMode: Text.Wrap
    }

    TextField {
        id: textField
        Layout.fillWidth: true

        readOnly: root.dataObject.readOnly
        enabled: !root.dataObject.readOnly
        text: undefined !== root.dataObject.value ? root.dataObject.value.toString() : ""

        Binding {
            target: root.dataObject
            property: "value"
            value: textField.text
            when: !root.dataObject.readOnly
        }
    }
}


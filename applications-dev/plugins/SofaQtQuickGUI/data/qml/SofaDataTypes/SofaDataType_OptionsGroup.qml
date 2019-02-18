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
import SofaBasics 1.0

/***************************************************************************************************
  *
  * A widget dedicated to edit Data<OptionGroup> object.
  * an optiongroups have a dedicated property holding the possible choices represented with
  * string.
  * To access these choices you can use:
  *   - root.dataObject.properties.choices
  *
  *************************************************************************************************/
ComboBox {
    id: root

    property var dataObject: null

    enabled: !dataObject.readOnly

    model: {
        return dataObject.properties.choices;
    }

    onModelChanged: {
        var values = model.toString().split(',');
        for (var idx = 0 ; idx < values.length ; idx++)
            if (values[idx] === dataObject.value)
                currentIndex = idx;
    }

    onCurrentTextChanged: {
        dataObject.value = currentText;
        dataObject.upload();
    }
}

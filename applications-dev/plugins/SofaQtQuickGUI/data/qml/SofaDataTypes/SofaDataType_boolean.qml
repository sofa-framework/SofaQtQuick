/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - initial version from Anatoscope
    - damien.marchal@univ-lille.fr
********************************************************************/

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.4
import SofaBasics 1.0
import Sofa.Core.SofaData 1.0

ColumnLayout {
    id: root
    spacing: 0

    property SofaData sofaData: null
    property int refreshCounter : 0
    onRefreshCounterChanged:
    {
        control.checked = sofaData.value
    }

    CheckBox {
        id: control
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        enabled: !sofaData.properties.readOnly
        checked: sofaData.value

        onCheckedChanged:
        {
            if (sofaData.value !== checked)
                sofaData.value = checked;
        }
    }
}

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
import QtQuick.Controls 2.4
import SofaBasics 1.0
import Sofa.Core.SofaData 1.0


SpinBox
{
    id: spinbox
    property SofaData sofaData: null
    property var properties: sofaData.properties

    enabled: !sofaData.readOnly
    precision: properties["precision"]
    step: properties["step"] !== undefined ? properties["step"] : 1

    Item
    {
        id: bidirectionalLink
        property var value
        property string src: "none"

        onValueChanged:
        {
            console.log("SID: " + bidirectionalLink.src)
            if(bidirectionalLink.src==="right")
                sofaData.value=value
            if(bidirectionalLink.src==="left")
                spinbox.value=value
        }

        Connections
        {
            target: sofaData
            onValueChanged: {
                console.log("sofaData changed")
                bidirectionalLink.setLeftValue(sofaData.value)
            }
        }

        Connections
        {
            target: spinbox
            onValueChanged: {
                console.log("spinbox changed")
                bidirectionalLink.setRightValue(spinbox.value)
            }
        }


        function setLeftValue(newValue)
        {
            if(src!=="none")
                return
            console.log("LeftChanged" + newValue )
            src="left"
            value = newValue
            src="none"
        }

        function setRightValue(newValue)
        {
            if(src!=="none")
                return
            console.log("RightChanged" + newValue )

            //left.when = true
            src="right"
            value = newValue
            src="none"

            //left.when = false
        }
    }

    //    Connections
    //    {
    //        target: sofaData
    //        onValueChanged: {
    //            value=sofaData.value
    //        }
    //    }

    //    Connections
    //    {
    //        target: spinbox
    //        onValueEditted: function(newvalue)
    //        {
    //            console.log("COUCOUC2 " + newvalue)
    //            sofaData.value = newvalue
    //        }
    //    }
}

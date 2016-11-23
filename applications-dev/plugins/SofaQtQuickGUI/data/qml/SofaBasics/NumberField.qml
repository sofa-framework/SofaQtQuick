/*
Copyright 2016, CNRS

Contributors:
   damien.marchal@univ-lille1.fr

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

import QtQuick 2.5
import QtQuick.Controls 1.3
import SofaApplication 1.0
import SofaData 1.0


TextField {
    id: root

    property var value : 0
    property var minvalue : 0
    property var maxvalue : 0
    property int decimals: 0
    property bool isInt : decimals == 0
    property int minVisibledecimals : (decimals==0)? 0 : 5
    property int maxVisibledecimals : (decimals==0)? 0 : 10


    validator: decimals > 0 ? doubleValidator : intValidator

    Component.onCompleted: {
        download();
        horizontalAlignment = (width<textmetrics.width)? TextInput.AlignLeft : TextInput.AlignRight
    }

    TextMetrics
    {
        id : textmetrics
        font : root.font
        text : root.text
    }

    onValueChanged: {
        download()
    }

    onActiveFocusChanged: function(){
        if(activeFocus){
            if(decimals!=0)
                root.text = Number(value).toFixed(maxVisibledecimals) ;
            else
                root.text = Number(value).toFixed(0) ;


            if(width < textmetrics.width){
                horizontalAlignment = TextInput.AlignLeft
            }else{
                horizontalAlignment = TextInput.AlignLeft
            }
            select(text.length,0)
        }else{
            upload()
            download()
            textmetrics.text = root.text

            if(width < textmetrics.width){
                cursorPosition = 0
                horizontalAlignment = TextInput.AlignLeft
            }else{

                cursorPosition = maxVisibledecimals
                horizontalAlignment = TextInput.AlignRight
            }
        }
    }

    onAccepted: function(){
        root.focus=false
    }

    property var intValidator: IntValidator {
        //Component.onCompleted: {
        //    if(undefined !== minvalue)
        //        bottom = min;
        //}
    }

    property var doubleValidator: DoubleValidator {
        //decimals: root.decimals
    }

    ToolTip {
        delay: 300
        anchors.fill: parent
        description: Number(value).toFixed(decimals)
    }

    /***************************************************************
     The formatting rules is inspired by Blender.
     - the dot '.' is at a fixed position so that row of numbers are aligned
     - there is 5 decimal number at right
     - there is at least 4 decimal number at right
    ***************************************************************/
    function download() {
        console.log("START LOADING")
        console.log("VALUS IS " +value)

        if(decimals!=0)
            root.text = Number(value).toFixed(minVisibledecimals) ;
        else
            root.text = Number(value).toFixed(0) ;
        console.log("RES")

        cursorPosition = 0;
    }

    function upload() {

        var oldValue = Number(Number(value).toFixed(decimals));
        var newValue = Number(Number(root.text).toFixed(decimals));

        if(oldValue !== newValue){
            value = root.text;
        }
    }


}

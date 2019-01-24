/*
Copyright 2016, CNRS

Author: damien.marchal@univ-lille1.fr

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
import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import SofaBasics 1.0
import SofaApplication 1.0
import SofaComponent 1.0
import SofaInspectorDataListModel 1.0

/*
  |--------------------------------------------------------|
  | Name of your Panel                                              |
  |--------------------------------------------------------|
  | Whatever you want to put in
  |--------------------------------------------------------|

*/
Rectangle{
    color: "lightgrey"
    clip: true
    anchors.fill: parent
    property var sofaScene: SofaApplication.sofaScene
    property var sofaSelectedComponent: sofaScene ? sofaScene.selectedComponent : null

    Column{
        width: parent.width
        height: parent.height
        //clip:true

        Rectangle{
            width: parent.width
            height: 20
            color: "darkgrey"
            Text{
                text : "MyBasePanelExample"
                font.pixelSize: 14
                font.bold: true
            }
        }

        Rectangle{
            width: parent.width
            height: 20
            color: "darkgrey"
            Text{
                text : "M2 ZZZZZZZZZZZZZZZZ"
                font.pixelSize: 14
                font.bold: true
            }
        }

        TextField {
            id : textfield

            property var value : 0.12345
            text : Number(value).toFixed(5)
            horizontalAlignment: TextInput.AlignRight


            onActiveFocusChanged: function(){
                if(activeFocus){
                    horizontalAlignment = TextInput.AlignLeft
                    text = Number(value).toFixed(10)
                    cursorPosition = 0
                    selectAll()
                }else{
                    horizontalAlignment = TextInput.AlignRight
                    value = parseFloat(text)
                    text = Number(value).toFixed(5)
                    cursorPosition = 10
                }
            }

            onAccepted: function(){
                textfield.focus=false
            }

            MouseArea
            {
               anchors.fill: parent
               onClicked : function(me) {
                    if(!textfield.focus){
                        textfield.focus = true
                    }else{
                        textfield.focus=false
                    }
                }
            }

        }

        //        //// I'm make a new text field.
        //        Item{
        //            width: 100
        //            property alias textheight : textfield.height

        //            Rectangle
        //            {
        //                width : parent.width
        //                height : textfield.height
        //                color : "white"
        //            }
        //            TextInput
        //            {
        //                width : parent.width
        //                id : textfield
        //                property double value : 0.5
        //                property bool isEdditing : false
        //                property int targetCursorPosition : 0

        //                text : Number(value).toFixed(5)

        //                horizontalAlignment : TextInput.AlignRight
        //                /*style: TextFieldStyle {
        //                    textColor: "black"
        //                    background: Rectangle {
        //                        radius: 4
        //                        implicitWidth: 100
        //                        implicitHeight: 24
        //                        border.color: "#333"
        //                        border.width: 1
        //                    }
        //             }*/

        //                onAccepted: function(){
        //                    console.log("ACCEPTeD "+isEdditing)
        //                    //isEdditing = false
        //                    focus = false
        //                    //value = parseFloat(text)
        //                    //text : Number(value).toFixed(5)
        //                    //focus = false
        //                }

        //                onIsEdditingChanged: function(){
        //                    console.log("CHANGED")
        //                }

        //                onActiveFocusChanged: function(f){
        //                    console.log("--== focus changed ==--")
        //                    if(focus){
        //                        console.log("HAS FOCUS: "+value)
        //                        text = Number(value).toFixed(10)
        //                        horizontalAlignment = TextInput.AlignLeft ;
        //                        cursorPosition = targetCursorPosition

        //                    }else{
        //                        console.log("LEAVE FOCUS: "+value)
        //                        console.log("LEAVE FOCUS text: "+text)
        //                        value = parseFloat(text)
        //                        console.log("LEAVE FOCUS new value: "+value)

        //                        horizontalAlignment = TextInput.AlignRight ;
        //                        text = Number(value).toFixed(5)
        //                    }
        //                }
        //            }
        //        }
        //        MouseArea
        //        {

        //            Item{
        //                id:nothing
        //            }

        //            anchors.fill: parent
        //            property bool sliding : false
        //            property int ix : 0
        //            property int iy : 0
        //            property int px : 0
        //            property int py : 0
        //            property int textpos : 0
        //            propagateComposedEvents: false
        //            preventStealing: true
        //            onPressed: function(me){
        //                //me.accepted=false
        //                ix = me.x
        //                iy = me.y
        //                px = me.x
        //                py = me.y
        //                textpos = textfield.positionAt(me.x, me.y)
        //                console.log("CursorPosition"+textpos)
        //            }

        //            onCanceled: function(me){
        //                console.log("DDDDDD")
        //            }

        //            onExited : function(me){
        //                console.log("EXITTED")
        //            }

        //            onClicked : function(me) {
        //                console.log("onCLICKED")
        //                if(!textfield.focus){
        //                    textfield.targetCursorPosition = textpos
        //                    textfield.forceActiveFocus()
        //                    textfield.focus = true
        //                    textfield.isEdditing=true;


        //                    //textfield.text = Number(textfield.value).toPrecision(5)
        //                }else{
        //                    textfield.focus=false
        //                    textfield.isEdditing=false
        //                    //textfield.text = Number(textfield.value).toFixed(5)
        //                }
        //            }

        //            onDoubleClicked: function(me){
        //                textfield.selectAll()
        //            }

        //            onPositionChanged : function(me){
        //                //if(textfield.isEdditing){
        //                //    me.accepted=false
        //                //}

        //                if( sliding==false){
        //                    if(Math.abs(ix-me.x) > 20 ){
        //                        sliding = true
        //                        px = me.x
        //                    }
        //                }else{
        //                    var dx = Math.abs(px-me.x)
        //                    if((px-me.x)>0){
        //                        textfield.value -= dx*0.01
        //                    }else{
        //                        textfield.value += dx*0.01
        //                    }
        //                    px = me.x
        //                    textfield.text = Number(textfield.value).toFixed(5)
        //                }
        //            }

        //            onReleased: function(me){
        //                console.log("onRelease...")
        //                if(sliding){
        //                    sliding = false
        //                    //textfield.focus=false
        //                }else{

        //                }
        //            }


        //        }


    }
}

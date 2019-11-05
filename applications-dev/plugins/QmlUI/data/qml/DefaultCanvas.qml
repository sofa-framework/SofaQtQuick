import QtQuick.Controls 2.4
import QtQuick 2.0
import SofaBasics 1.0

Rectangle {
    anchors.centerIn: parent
    width: 100
    height: 100
    color: "red"
    Button {
	text: "Hello World!"
	anchors.centerIn: parent
	onClicked: {
	    if (text === "Hello World!")
		text = "CLICK ME"
	    else
		text = "Hello World!"
	}
    }
}

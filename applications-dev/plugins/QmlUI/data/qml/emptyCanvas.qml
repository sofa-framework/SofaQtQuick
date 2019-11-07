import QtQuick.Controls 2.4
import QtQuick 2.0
import SofaBasics 1.0

SofaCanvas {
    width: 150
    height: 100
    color: "red"
    Button {
        text: "Hello World!"
	      anchors.centerIn: parent
	      onClicked: {
	          if (text === "Hello World!")
                text = "ComponentName:" + self.getName()
	          else
                text = "Hello World!"
	      }
    }
}

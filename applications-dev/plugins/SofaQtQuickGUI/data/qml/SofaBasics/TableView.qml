import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4 as QQC1
import SofaBasics 1.0 as SB
import Sofa.Core.SofaData 1.0
import SofaApplication 1.0
import SofaColorScheme 1.0

QQC1.TableView {
    id: tableView

    Component {
        id: lineIdxColumn
        QQC1.TableViewColumn {
            movable: false
            resizable: false
            horizontalAlignment: Text.AlignHCenter
            width: 58
        }
    }
    Component {
        id: contentColumn
        QQC1.TableViewColumn {
            movable: false
            resizable: false
            horizontalAlignment: Text.AlignHCenter
            width: (tableView.width - lineIdxColumn.width) / (tableView.columnCount - 1)
        }
    }
    model: ListModel {
        id: libraryModel
        ListElement {
            title: "A Masterpiece"
            author: "Gabriel"
        }
        ListElement {
            title: "Brilliance"
            author: "Jens"
        }
        ListElement {
            title: "Outstanding"
            author: "Frederik"
        }
        Component.onCompleted: {
            addColumn(lineIdxColumn.createObject(tableView, {"title":"Title",  "role": "title"  }));
            addColumn(contentColumn.createObject(tableView, {"title":"Author", "role": "author" }));
        }
    }




    /// Style:
    rowDelegate: Rectangle {
        color: styleData.selected ? "#82878c" : styleData.alternate ? SofaApplication.style.alternateBackgroundColor : SofaApplication.style.contentBackgroundColor
    }
    headerDelegate: GBRect {
        color: "#757575"
        border.color: "black"
        borderWidth: 1
        borderGradient: Gradient {
            GradientStop { position: 0.0; color: "#5a5a5a" }
            GradientStop { position: 1.0; color: "#3c3c3c" }
        }
        height: 20
        width: textItem.implicitWidth
        Text {
            id: textItem
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: styleData.textAlignment
            anchors.leftMargin: 12
            text: styleData.value
            elide: Text.ElideRight
            color: textColor
            renderType: Text.NativeRendering
        }
    }
}

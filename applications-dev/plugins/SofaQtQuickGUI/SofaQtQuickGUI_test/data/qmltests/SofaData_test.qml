import QtQuick 2.12
import SofaFactory 1.0
import Sofa.Core.Node 1.0
import QmlGTestCase 1.0

QmlGTestCase
{
    property var node : SofaFactory.createNode()
    Connections {
        target: node
        valueChanged:
        {
            console.log("z==================")
        }
    }

    function tst_property_value()
    {
        assert_eq(node.getData("name").value, "unnamed")
    }

    function tst_property_setValue()
    {
        node.getData("name").value = "newValue"
        assert_eq(node.getData("name").value, "newValue")
    }


}

import QtQuick 2.12
import Sofa.Core.SofaFactory 1.0
import Sofa.Core.SofaNode 1.0
import Sofa.Core.SofaData 1.0
import QmlGTestCase 1.0

QmlGTestCase
{
    property var node : SofaFactory.createNode()
//    Connections {
//        target: node
//        valueChanged:
//        {
//            console.log("z==================")
//        }
//    }

    function tst_property_value()
    {
        assert_eq(node.getData("name").value, "unnamed")
    }

    function tst_property_setValue()
    {
        node.getData("name").value = "newValue"
        assert_eq(node.getData("name").value, "newValue")
    }


    function tst_function_createChild()
    {
        tst_property_setValue()
        var newNode = node.addChild("NEWNODE")
        assert_eq(newNode.getData("name").value, "NEWNODE")
    }
    function tst_function_getChild()
    {
        tst_function_createChild()
        var newNode = node.getChild("NEWNODE")
        assert_eq(newNode.getData("name").value, "NEWNODE")
    }

    function tst_function_getFirstParent()
    {
        tst_function_createChild()
        var newNode = node.getChild("NEWNODE")
        assert_eq(newNode.getFirstParent().getData("name").value, "newValue")
    }

    function tst_function_getRoot()
    {
        node.getData("name").value = "root"
        var n1 = node.addChild("Node1")
        var n2 = n1.addChild("Node2")
        assert_eq(n2.getRoot().getData("name").value, "root")
        assert_eq(n1.getRoot().getData("name").value, "root")
    }

    function tst_function_addChild()
    {
        node.getData("name").value = "root"
        var n1 = node.addChild("Node1")
        var n2 = node.addChild("Node2")
        n1.addChild(n2)
        assert_eq(n1.getChild("Node2").getData("name").value, "Node2")
    }

    function tst_getInvalidData()
    {
        assert_throw(function (){console.log(node.getData("thisLinkDoesNotExists"))});
    }
}

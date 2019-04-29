import QtQuick 2.12
import SofaFactory 1.0
import Sofa.Core.SofaNode 1.0
import QmlGTestCase 1.0

QmlGTestCase
{
    function tst_gets()
    {
        var n = SofaFactory.createNode()
        assert_eq(n.getName(), "unnamed")
        assert_eq(n.getClassName(), "DAGNode")
        assert_eq(n.getTemplateName(), "")
    }

    function tst_getData()
    {
        var n = SofaFactory.createNode()
        assert_throw(function(){ n.getData("unknown")} )
        assert_eq(n.getData("name").value, "unnamed")
    }

    function tst_createChild()
    {
        var n = SofaFactory.createNode()
        var c = n.createChild("Child")
        assert_eq(c.getData("name").value, "Child")
    }

    function tst_getChildren()
    {
        var n = SofaFactory.createNode()
        var c1 = n.createChild("Child1")
        var c2 = n.createChild("Child2")
        assert_eq(n.getChildren().size(), 2)
    }
}

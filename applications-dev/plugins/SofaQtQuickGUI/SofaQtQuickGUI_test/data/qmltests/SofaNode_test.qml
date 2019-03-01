import QtQuick 2.12
import SofaFactory 1.0
import Sofa.Core.Node 1.0
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
        assert_eq(n.getData("unknown"), null)
        assert_eq(n.getData("name").value, "unnamed")
    }
}

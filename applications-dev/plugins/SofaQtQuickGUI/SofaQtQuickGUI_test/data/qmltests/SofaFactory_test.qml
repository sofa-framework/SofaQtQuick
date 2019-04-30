import Sofa.Core.SofaFactory 1.0
import QmlGTestCase 1.0

QmlGTestCase
{
    function tst_createNode()
    {
        var n = SofaFactory.createNode();
        assert_eq(n.getName(), "unnamed");
        assert_eq(n.getClassName(), "DAGNode");
    }

    function tst_createObject()
    {
        var n = SofaFactory.createNode();
        assert_eq(n.getName(), "unnamed");
        assert_eq(n.getClassName(), "DAGNode");
    }
}

import Sofa.Core.SofaFactory 1.0
import Sofa.Core.SofaNode 1.0
import QmlGTestCase 1.0

QmlGTestCase
{
    function tst_createInit()
    {
        var n = SofaFactory.createNode();
        assert_eq(n.getName(), "unnamed");
        assert_eq(n.getClassName(), "DAGNode");
        n.init();
        n.reinit();
    }
}

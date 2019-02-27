import QtQuick 2.12
import SofaFactory 1.0
import Sofa.Core.Node 1.0

Item
{
    function assert_eq(a,b)
    {
        if(a!==b)
            console.exception("FAILURE: expecting '"+b+"' got '"+a+"'");
    }
    function assert_neq(a,b)
    {
        if(a===b)
            console.exception("FAILURE: expecting '"+b+"' not '"+a+"'");
    }

    function expect_eq(a, b)
    {
        console.log("COUCOU ... affiche");
        console.exception("FAILURE: expecting "+a+" got "+b);
    }

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

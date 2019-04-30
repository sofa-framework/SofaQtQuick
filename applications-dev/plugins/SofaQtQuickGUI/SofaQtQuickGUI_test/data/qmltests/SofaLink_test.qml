import QtQuick 2.12
import Sofa.Core.SofaFactory 1.0
import Sofa.Core.SofaLink 1.0
import QmlGTestCase 1.0

QmlGTestCase
{
    property var node : SofaFactory.createNode()
    property var node2 : node.createChild("Child")


    function tst_getInvalidLink()
    {
        assert_throw(function (){console.log(node.getLink("thisLinkDoesNotExists"))});
    }

    function tst_getSize()
    {
        node.createChild("Child2");
        console.log(node.getLink("object").getSize())
    }

    function tst_getLinkedData()
    {
        console.log(node.getLink("object").getLinkedBase())
    }

    function tst_getLinkedBase()
    {
        console.log(node.getLink("object").getLinkedData())
    }

    function tst_getLinkedPath()
    {
        console.log(node.getLink("object").getLinkedPath())
    }


}

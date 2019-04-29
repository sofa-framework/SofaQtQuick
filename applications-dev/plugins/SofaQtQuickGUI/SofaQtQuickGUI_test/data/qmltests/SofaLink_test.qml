import QtQuick 2.12
import SofaFactory 1.0
import Sofa.Core.SofaLink 1.0
import QmlGTestCase 1.0

QmlGTestCase
{
    property var node : SofaFactory.createNode()

    function tst_getLink()
    {
        console.log(node.getLink("object"))
    }

    function tst_getInvalidLink()
    {
        assert_throw(function (){console.log(node.getLink("thisLinkDoesNotExists"))});
    }

}

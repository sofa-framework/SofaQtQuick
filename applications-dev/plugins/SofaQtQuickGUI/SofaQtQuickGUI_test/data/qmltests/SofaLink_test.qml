import QtQuick 2.12
import Sofa.Core.SofaFactory 1.0
import Sofa.Core.SofaLink 1.0
import QmlGTestCase 1.0

QmlGTestCase
{
    property var rootNode :
    {
        var node = SofaFactory.createNode()

        var parent = node.addChild("Parent")
        var dofs = parent.addObject("MechanicalObject",
                                     { name : "dofs" ,
                                       template : "Rigid3d"
                                     })
        var child = parent.addChild("Visual")
        var slavedofs = child.addObject("MechanicalObject",
                                          {name : "slavedofs",
                                              template : "Rigid3d"
                                          })
        var mapping = child.addObject("RigidRigidMapping",
                                        {name : "mapping",
                                         template : "Rigid3d,Rigid3d",
                                         input : "@../dofs",
                                         output : "@./slavedofs"
                                        })
        node.init()
        return node
    }

    function tst_getInvalidLink()
    {
        assert_throw(function (){console.log(rootNode.getLink("thisLinkDoesNotExists"))});
    }

    function tst_getSize()
    {
        rootNode.addChild("Child2");
        assert_eq(rootNode.children().size(),2)
    }

    function tst_getLinkedData()
    {
    }

    function tst_getLinkedBase()
    {
        assert_eq(rootNode.getChild("Parent").getChild("Visual").getObject("mapping").findLink("input").getLinkedBase().getPathName(), "/Parent/dofs")
        assert_eq(rootNode.getChild("Parent").getChild("Visual").getObject("mapping").findLink("output").getLinkedBase().getPathName(), "/Parent/Visual/slavedofs")
    }

    function tst_getLinkedPath()
    {
        assert_eq(rootNode.getChild("Parent").getChild("Visual").getObject("mapping").findLink("input").getLinkedPath(), "@../dofs")
        assert_eq(rootNode.getChild("Parent").getChild("Visual").getObject("mapping").findLink("output").getLinkedPath(), "@./slavedofs")
    }


}

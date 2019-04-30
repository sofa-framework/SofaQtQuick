import QtQuick 2.12
import Sofa.Core.SofaFactory 1.0
import Sofa.Core.SofaLink 1.0
import QmlGTestCase 1.0

QmlGTestCase
{
    property var rootNode :
    {
        var node = SofaFactory.createNode()

        var parent = node.createChild("Parent")
        var dofs = parent.createObject("MechanicalObject",
                                     { name : "dofs" ,
                                       template : "Rigid3d"
                                     })
        var child = parent.createChild("Visual")
        var slavedofs = child.createObject("MechanicalObject",
                                          {name : "slavedofs",
                                              template : "Rigid3d"
                                          })
        var mapping = child.createObject("RigidRigidMapping",
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
        rootNode.createChild("Child2");
        assert_eq(rootNode.getChildren().size(),2)
    }

    function tst_getLinkedData()
    {
    }

    function tst_getLinkedBase()
    {
        assert_eq(rootNode.getChild("Parent").getChild("Visual").getObject("mapping").getLink("input").getLinkedBase().getPathName(), "/Parent/dofs")
        assert_eq(rootNode.getChild("Parent").getChild("Visual").getObject("mapping").getLink("output").getLinkedBase().getPathName(), "/Parent/Visual/slavedofs")
    }

    function tst_getLinkedPath()
    {
        assert_eq(rootNode.getChild("Parent").getChild("Visual").getObject("mapping").getLink("input").getLinkedPath(), "@../dofs")
        assert_eq(rootNode.getChild("Parent").getChild("Visual").getObject("mapping").getLink("output").getLinkedPath(), "@./slavedofs")
    }


}

import Sofa
import os, numpy, sys
from Compliant import StructuralAPI
from Compliant import Tools as CompliantTools
from SofaPython import Quaternion, Tools

currentdir = CompliantTools.path( __file__ )

N = 10

bodies=[]

def createScene(root):
    
    root.createObject('PythonScriptController', name="moveController", filename=__file__, classname="MoveController")
    
    root.createObject('RequiredPlugin',name='Compliant')
    root.createObject('RequiredPlugin',name='ContactMapping')
    
    root.createObject('VisualStyle',name='VisualStyle', displayFlags="hideMechanicalMappings showCollisionModels")

    root.createObject('CompliantImplicitSolver')
    #root.createObject('CompliantPseudoStaticSolver',iterations=1,velocityFactor=0)
    #root.createObject('CgSolver',iterations="25", precision="1e-6")
    root.createObject('SequentialSolver',iterations="100", precision="1e-6")
    
    root.gravity=[[0,0,0]]
    
    global bodies
    
    # bodies
    for i in xrange(N):
        r = StructuralAPI.RigidBody( root, 'P'+str(i) )

        r.setManually(  [i,0,0,0,0,0,1], 1, [1,1,1] )

        r.addCollisionMesh( 'mesh/torus.obj', [.1, .1, .1], [0,0,0,0,0,0,1], "_" + str(i))

        model = r.collision.node.getObject("model")

        # define the body index in the name of the selectable component (here a collision model) to be able to retrieve the index from its name
        model.name = model.name + "_" + str(i)

        r.collision.node.createObject('NormalsFromPoints', name="NormalsFromPoints", template="Vec3", position="@dofs.position", triangles="@loader.triangles", quads="@loader.quads")
        
        bodies.append(r)

class MoveController(Sofa.PythonScriptController) :
    node = None
            
    def initGraph(self, node):
        self.node = node

    def getTransformation(self, index):
        return bodies[index].dofs.position[0]

    def setTransformation(self, index, transformation):
        bodies[index].dofs.position = [transformation[0], transformation[1], transformation[2], transformation[3], transformation[4], transformation[5], transformation[6]]
        bodies[index].node.init()

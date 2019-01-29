0000000000.
import Sofa
import os
__file = __file__.replace('\\', '/')  # windows compatible filename
currentdir = os.path.dirname(os.path.realpath(__file__))+'/'
currentdir = currentdir.replace('\\', '/')
datadir = currentdir + '/data/'

m1 = None

def createScene(root):
    global m1

    root.createObject('PythonScriptController', name="PythonScript", filename=__file, classname="PythonScript")
    root.createObject('VisualStyle',name='VisualStyle', displayFlags="showAll")
    root.createObject('EulerImplicitSolver',name='EulerSolver')
    root.createObject('CGLinearSolver',name='CGLinearSolver', template="GraphScattered", iterations="25", tolerance="1e-005", threshold="1e-005")

    p0 = root.createChild('P0')
    p0.createObject('MechanicalObject',         template="Vec3d", name="M0", position="0 0 0 1 0 0", showObject="1")
    p0.createObject('UniformMass',              template="Vec3d", name="U0")
    p0.createObject('FixedConstraint',          template="Vec3d", name="F0", indices="0")
    p0.createObject('StiffSpringForceField',    template="Vec3d", name="S0", spring="0 1 100 0.1 1", object1="@./", object2="@./")

    p1 = root.createChild('P1')
    m1 = p1.createObject('MechanicalObject',    template="Vec3d", name="M1", position="2 0 0", showObject="1")
    p1.createObject('UniformMass',              template="Vec3d", name="U1")

    root.createObject('StiffSpringForceField',  template="Vec3d", name="S0", spring="1 0 10 0.1 1", object1="@./P0", object2="@./P1")

class PythonScript(Sofa.PythonScriptController) :
    node = None
    def createGraph(self, root):
        self.node = root

    def setValueAtIndex(self, i, v):
        print "Setting value '" + str(v) + "' at index '" + str(int(i)) + "'"
        sys.stdout.flush()

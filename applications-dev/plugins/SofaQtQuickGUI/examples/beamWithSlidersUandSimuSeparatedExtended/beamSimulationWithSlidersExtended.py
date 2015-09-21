0000000000.
import sys,os
import Sofa
import Flexible.IO
import math

# main path
__file = __file__.replace('\\', '/')  # windows compatible filename
currentdir = os.path.dirname(os.path.realpath(__file__))+'/'
currentdir = currentdir.replace('\\', '/')
datadir = currentdir + '/data/'


def createScene(root_node) :
    # Required plugin
    root_node.createObject('RequiredPlugin', pluginName='SofaPython')
    root_node.createObject('RequiredPlugin', pluginName='Flexible')
    root_node.createObject('RequiredPlugin', pluginName='Compliant')
    root_node.createObject('RequiredPlugin', pluginName='ModalSubspace')
    root_node.createObject('PythonScriptController', name='script', filename=__file, classname='MyClass')

class MyClass(Sofa.PythonScriptController):

    def createGraph(self, root):
        # Variable
        #self.E_t = 0
        #self.E_t_dt = 0
        self.percent = 0.05
        self.parentNode = root
        self.n = 0
        self.r = int( self.percent * 675 )
        self.t = 0
        self.index = 0
        self.table = [[0.0]]*9*self.r
        self.amplitude = [[0.0]]*9*self.r

        ###############################################################################
        self.parentNode.findData('gravity').value = '0.0, 0.0, 9.81'
        self.parentNode.createObject('VisualStyle',  displayFlags='showBehavior hideVisual')

        # On doit charger le plugin ModalSubspace car ensuite on utilise...
        # ...le SubspaceMapping.inl
        # Pour animer la scene
        self.parentNode.createObject('CompliantImplicitSolver')
        #self.parentNode.createObject('EulerSolver')
        self.parentNode.createObject('LDLTSolver' , printLog=0)
        self.parentNode.createObject('LDLTResponse' , printLog=0)
        #
        #
        main = self.parentNode.createChild('Main')
        #
        # position = "autant de zeros que de modes propres choisis"
        base = '0'+' '
        posDDL = base*(9*self.r-1)+'0'
        main.createObject('MechanicalObject' ,  name='master' , template='Vec1d' , position=posDDL)
        #
        #
        # <Node name = "child">
        child = main.createChild('Child')
        # Subdivided cube
        child.createObject('RegularGridTopology' , name='grid' , n='10 5 5' , min='0 0 0' , max='5 1 1')
        child.createObject('MechanicalObject', name='slave' , template='Vec3d' , showObject='true')
        child.createObject('UniformMass' , name='themass' , totalMass='1')
        child.createObject('HexahedronFEMForceField' , youngModulus='100' , poissonRatio='0.2')
        child.createObject('SubspaceMapping' , template='Vec1d,Vec3d' , input='@../master' , output='@slave' , filename ='../../../src/sofa-dev/applications-dev/plugins/ModalSubspace/matrices/maMatriceUextended.txt')
        return self.parentNode

    def initGraph(self,root):
        self.node = root
        self.MechanicalState = root.getObject('Main/master')
        return 0

    def setAmplitudeToOscilloAtIndex(self, i, v):
        #print "Setting value '" + str(v) + "' at index '" + str(int(i)) + "'"
        self.amplitude[int(i)] = [10*v]
        sys.stdout.flush()

    def onBeginAnimationStep(self, dt):
        for i in range(0,9*self.r):
            #print 'before --> ', self.table[i][0]
            #self.table[i] = [self.amplitude[i][0]*math.sin(3*self.t)]
            self.table[i] = self.amplitude[i][0]
            #self.table[i] = [self.amplitude[i][0]]
            #print 'after --> ', self.table[i][0]
            #print type(self.table[i])
            #print i , "-->" , self.table[i] , "sin -->" , math.sin(3*self.t)

        self.MechanicalState.position = self.table
        #print self.MechanicalState.position
        self.t = self.t + dt

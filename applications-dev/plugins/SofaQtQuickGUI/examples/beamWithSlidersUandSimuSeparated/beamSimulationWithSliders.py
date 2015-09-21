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
        self.percent = 0.05
        self.parentNode = root
        self.n = 0
        self.r = int( self.percent * 675 )
        self.t = 0
        self.index = 0
        self.table = [[0.0]]*self.r
        self.amplitude = [[0.0]]*self.r

        ###############################################################################
        self.parentNode.findData('gravity').value = '0.0, 0.0, 0.0'
        self.parentNode.createObject('VisualStyle',  displayFlags='showBehavior hideVisual')

        # On doit charger le plugin ModalSubspace car ensuite on utilise...
        # ...le SubspaceMapping.inl
        # Pour animer la scene
        self.parentNode.createObject('CompliantImplicitSolver' , )
        #compliantModalAnalysis = self.parentNode.createObject('CompliantModalAnalysis')
        self.parentNode.createObject('LDLTSolver' , printLog=0)
        self.parentNode.createObject('LDLTResponse' , printLog=0)
        #
        #
        main = self.parentNode.createChild('Main')
        #
        # position = "autant de zeros que de modes propres choisis"
        base = '0'+' '
        posDDL = base*(self.r-1)+'0'
        main.createObject('MechanicalObject' ,  name='master' , template='Vec1d' , position=posDDL)
        #
        #
        child = main.createChild('Child')
        # Subdivided cube
        child.createObject('RegularGridTopology' , name='grid' , n='10 5 5' , min='0 0 0' , max='5 1 1')
        child.createObject('MechanicalObject', name='slave' , template='Vec3d' , showObject='true')
        child.createObject('UniformMass' , name='themass' , totalMass='1')
        hexahedronFEMForceField = child.createObject('HexahedronFEMForceField' , youngModulus='100' , poissonRatio='0.3')

        child.createObject('SubspaceMapping' , template='Vec1d,Vec3d' , input='@../master' , output='@slave' , filename ='../../../src/sofa-dev/applications-dev/plugins/ModalSubspace/matrices/maMatriceU.txt')
        return self.parentNode


    #postion fixee a tout instant
    def initGraph(self,root):
        self.node = root
        self.MechanicalState = root.getObject('Main/master')
        return 0

    def setAmplitudeToOscilloAtIndex(self, i, v):
        #print "Setting value '" + str(v) + "' at index '" + str(int(i)) + "'"
        self.amplitude[int(i)] = [v]
        sys.stdout.flush()

    def onBeginAnimationStep(self, dt):
        for i in range(0,self.r):
            #print 'before --> ', self.table[i][0]
            self.table[i] = [self.amplitude[i][0]]#*math.sin(3*self.t)]
            #self.table[i] = [self.amplitude[i][0]]
            #print 'after --> ', self.table[i][0]
            #print type(self.table[i])
            #print i , "-->" , self.table[i] , "sin -->" , math.sin(3*self.t)

        self.MechanicalState.position = self.table
        #print self.MechanicalState.position


    """
    # Position de depart fixee
    def initGraph(self,root):
        self.node = root
        self.MechanicalState = root.getObject('Main/master')
        return 0

    def setAmplitudeToOscilloAtIndex(self, i, v):
        self.amplitude[int(i)] = [v]
        sys.stdout.flush()

    def onBeginAnimationStep(self, dt):
        if self.t==0.0:
            for i in range(0,self.r):
                self.table[i] = [self.amplitude[i][0]]#*math.sin(3*self.t)]

            self.MechanicalState.position = self.table

        self.t = self.t + dt
        #print self.t
    """





    """
    timeVSpercentFile=None
    def onBeginAnimationStep(self, dt):
        global timeVSpercentFile
        dt = int(100*dt)
        #print self.t, " ", dt
        if self.t==0.0:
            timeVSpercentFile = open(currentdir + "/../../../Cubature_test/timeVSpercent.dat", "w")
            for i in range(0,self.r):
                #print 'before --> ', self.table[i][0]
                self.table[i] = [self.amplitude[i][0]]#*math.sin(3*self.t)]
                #self.table[i] = [self.amplitude[i][0]]
                #print 'after --> ', self.table[i][0]
                #print type(self.table[i])
                #print i , "-->" , self.table[i] , "sin -->" , math.sin(3*self.t)
            self.MechanicalState.position = self.table

    elif self.t==1:
        self.beginTime = time.time()
        print "begin= ", self.beginTime

    elif self.t==201:
        self.endTime = time.time()
        self.deltaTime = self.endTime-self.beginTime
        print "delta= ", self.deltaTime
        delta = str(self.deltaTime)
        timeVSpercentFile.write(delta)
        timeVSpercentFile.close()




    #print "force", len(self.MechanicalState2.force)
    #print self.MechanicalState.position
    #self.Cub = self.node.getObject('Main/Child/Behavior/Strain/cub')

    self.t = self.t + dt
    #print "time: ", time.time()
    print self.t

    #self.stepClicked = int(self.stepClicked + dt/dt)
    #self.Cub.nbStepClicked = self.stepClicked
    #self.Cub.stress = self.MechanicalState2.force
    """

import Sofa
import Flexible.IO
import math
import sys

def createCantileverBeam2(parentNode, name):
        #######################################
        percent = 0.05
        #######################################
        #
        #
        # <Node name="" gravity="0 0 0" dt="0.01">
        parentNode.findData('gravity').value = '0.0, 0.0, 0.0'
        # <VisualStyle displayFlags="showBehavior hideVisual" />
        parentNode.createObject('VisualStyle',  displayFlags='showBehavior hideVisual')
        # <RequiredPlugin name="Flexible" /> et SofaPython
        parentNode.createObject('RequiredPlugin', pluginName='SofaPython')
        parentNode.createObject('RequiredPlugin', pluginName='Flexible')
        # <RequiredPlugin name="Compliant" />
        parentNode.createObject('RequiredPlugin', pluginName='Compliant')
        # <RequiredPlugin name="ModalSubspace" />
        parentNode.createObject('RequiredPlugin', pluginName='ModalSubspace')
        # <CompliantModalAnalysis informationPercent="0.01" UMatrixFile="../../../src/sofa-dev/applications-dev/plugins/ModalSubspace/matrices/maMatriceU.txt"/>
        compliantModalAnalysis = parentNode.createObject('CompliantModalAnalysis', informationPercent=percent , UMatrixFile='../../../src/sofa-dev/applications-dev/plugins/ModalSubspace/matrices/maMatriceU.txt')
        compliantModalAnalysis.UextendedMatrixFile = '../../../src/sofa-dev/applications-dev/plugins/ModalSubspace/matrices/maMatriceUextended.txt'
        # <LDLTSolver printLog="0" />
        parentNode.createObject('LDLTSolver' , printLog=0)
        # Subdivided cube
        # <RegularGridTopology name="grid" n="10 5 5" min="0 0 0" max="5 1 1"  />
        regularGridTopology = parentNode.createObject('RegularGridTopology' , name='grid' , n='10 5 5' , min='0 0 0' , max='5 1 1')
        # <MechanicalObject name="DOF" template="Vec3d" />
        parentNode.createObject('MechanicalObject' , name='DOF' , template='Vec3d')
        # <UniformMass name="themass" totalMass="1" />
        parentNode.createObject('UniformMass' , name='themass' , totalMass='1')
        # maintain points of plane x=1 fixed
        # <BoxROI template="Vec3d" name="O_box_roi" box="-1 -0.01 -0.01  0.1 1.01 1.01 "  drawPoints="1" drawSize="30" />
        parentNode.createObject('BoxROI' , template='Vec3d' , name='O_box_roi' , box='-1 -0.01 -0.01  0.1 1.01 1.01 ' , drawPoints='1' , drawSize='30')
        # <FixedConstraint indices="@[-1].indices" />
        parentNode.createObject('FixedConstraint' , indices='@[-1].indices')
        # <HexahedronFEMForceField youngModulus="100" poissonRatio="0.3" />
        parentNode.createObject('HexahedronFEMForceField' , youngModulus='100' , poissonRatio='0.3')
        # Variable
        percent = compliantModalAnalysis.informationPercent
        n = regularGridTopology.n[0]
        print 'percent --> ', percent
        print 'n --> ', type(n), len(n), n

        r = int( percent * (3*n[0]*n[1]*n[2] - 75) )
        print 'r --> ', r
        return parentNode


def createScene(rootNode):
        createCantileverBeam2(rootNode , "Cantilever Beam for creating matrix")
        return rootNode

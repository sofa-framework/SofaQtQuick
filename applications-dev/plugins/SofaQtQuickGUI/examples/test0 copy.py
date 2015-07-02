import Sofa
import os, numpy, sys
from Compliant import StructuralAPI
from Compliant import Tools as CompliantTools
from SofaPython import Quaternion, Tools


from OpenGL.GLUT import *
from OpenGL.GLU import *
from OpenGL.GL import *


currentdir = CompliantTools.path( __file__ )

N = 10


# pairs (object,collision point)
SMILING_CURVE_POINTS = [(0,0),(1,0),(2,0),(3,0)]




bodies=[]
distances=[[]]
smilingConstraintDofs=[]




def createScene(root):
    
    root.createObject('PythonScriptController', name="moveController", filename=__file__, classname="MoveController")
    smilingCurveController = root.createObject('PythonScriptController', name="smilingCurveController", filename=__file__, classname="SmilingCurveController")
    
    root.createObject('RequiredPlugin',name='Compliant')
    root.createObject('RequiredPlugin',name='ContactMapping')
    
    root.createObject('VisualStyle',name='VisualStyle', displayFlags="hideMechanicalMappings showCollisionModels")

    root.createObject('CompliantImplicitSolver')
    #root.createObject('CompliantPseudoStaticSolver',iterations=1,velocityFactor=0)
    #root.createObject('CgSolver',iterations="25", precision="1e-6")
    root.createObject('SequentialSolver',iterations="100", precision="1e-6")
    
    root.gravity=[[0,0,0]]
    
    global bodies
    global distances
    
    # bodies
    for i in xrange(N):
        r = StructuralAPI.RigidBody( root, 'P'+str(i) )
               
        r.setManually(  [i,0,0,0,0,0,1], 1, [1,1,1] )
        
        r.addCollisionMesh( 'mesh/torus.obj', [.1, .1, .1] )
        
        r.collision.node.createObject('NormalsFromPoints', name="NormalsFromPoints", template="Vec3", position="@dofs.position", triangles="@loader.triangles", quads="@loader.quads")
        
        #r.node.createObject('FixedConstraint', indices="0")
               
        #r.dofs.showObject=True
        #r.dofs.showObjectScale=0.5
        
        bodies.append(r)
        
        distances.append([])

    
    # neighbourhood collisions
    for i in xrange(N-1):
        
        b0 = bodies[i]
        b1 = bodies[i+1]
        
        path0 = b0.collision.node.getPathName()
        path1 = b1.collision.node.getPathName()
        
        #print "@"+path0+"/. @"+path1+"/."
        
        distNode = b0.collision.node.createChild("distance"+"_"+str(i)+"_"+str(i+1))
        mo = distNode.createObject('MechanicalObject',template='Vec1')
        distNode.createObject('DistanceToMeshMultiMapping', template="Vec3,Vec1", name="mapping", input="@"+path0+"/. @"+path1+"/.", output="@.", restLengths=0,parallel=0,normals2="@"+path1+"/NormalsFromPoints.normals")
        #distNode.createObject('UniformCompliance', compliance=0,isCompliance=1)
        #distNode.createObject('UnilateralConstraint')
        
        b1.collision.node.addChild(distNode)
        
        distances[i].append( mo )
        distances[i+1].append( mo )


    # smiling curve plane constraints
    for i in xrange(4):
        projectionNode = bodies[i].collision.node.createChild( "smilingCurveProjection" )
        projectionNode.createObject('MechanicalObject', template = 'Vec3d', name = 'proj_dofs', showObject=True, showObjectScale=0.03, drawMode=2 )
        pttpm = projectionNode.createObject('ProjectionToTargetPlaneMapping', indices=SMILING_CURVE_POINTS[i][1], origins=Tools.listToStr( bodies[i].collision.dofs.position[SMILING_CURVE_POINTS[i][1]] ), normals="0 1 0" )
        smilingConstraintDofs.append( pttpm )


        constraintNode = projectionNode.createChild( "smilingCurveConstraint" )
        constraintNode.createObject('MechanicalObject', template = 'Vec1d', name = 'distance_dofs', position = '0'  )
        constraintNode.createObject('EdgeSetTopologyContainer', edges="0 1" )
        constraintNode.createObject('DistanceMultiMapping', template="Vec3d,Vec1d", name='mapping', input = '@../../. @../.', output = '@.', indexPairs="0 0  1 0", showObjectScale="0.01"   )
        constraintNode.createObject('UniformCompliance', name="ucomp" ,template="Vec1d", compliance=1e-10 )
        bodies[0].collision.node.addChild( constraintNode )


class MoveController(Sofa.PythonScriptController) :
    
    node = None
    currentIndex = 0
    penetration = 0.03
            
    def initGraph(self, node):
        self.node = node
        
    def getPenetration(self):
       return self.penetration
        
    def setPenetration(self,i):
        self.penetration=i

    def setCurrentIndex(self, i):
        i = numpy.clip(i, 0, N-1)
        #bodies[self.currentIndex].dofs.showObjectScale = 0.5
        self.currentIndex = i
        #bodies[self.currentIndex].dofs.showObjectScale = 0.8
       
    def remove(self):
        global bodies
        bodies[self.currentIndex].node.activated = not bodies[self.currentIndex].node.activated
        return
        
    def move(self,dir,inc):
        
        global bodies
        
        if not bodies[self.currentIndex].node.activated:
            return
        
        dir = numpy.clip(dir, 0, 2)
        
        pos = bodies[self.currentIndex].dofs.position
        pos[0][dir] += inc
        bodies[self.currentIndex].dofs.position = pos
        
        bodies[self.currentIndex].node.init()
        
        global distances
        for d in distances[self.currentIndex]:
            while min([a[0] for a in d.position])<-self.penetration :
                pos[0][dir] -= inc*0.01
                #print min([a[0] for a in d.position])
                #sys.stdout.flush()
                bodies[self.currentIndex].dofs.position = pos
                bodies[self.currentIndex].collision.node.init()
                
        
        bodies[self.currentIndex].node.init()

        # update smiling curve constraint (plane position)
        if not self.node.animate:
            for i in xrange(4):
                if self.currentIndex == SMILING_CURVE_POINTS[i][0]:
                    smilingConstraintDofs[i].origins = Tools.listToStr( bodies[i].collision.dofs.position[SMILING_CURVE_POINTS[i][1]] )

        

    def draw(self):
        global bodies
        p = bodies[self.currentIndex].dofs.position[0]
        
        m = Quaternion.to_matrix( p[3:7] )
        m = numpy.vstack([m, [0,0,0]])
        m = numpy.hstack([m, [[p[0]],[p[1]],[p[2]],[1]] ])
        # todo scale
        
        
        pcol = bodies[self.currentIndex].collision.dofs.position
        min_x = numpy.min(pcol,axis=0)
        max_x = numpy.max(pcol,axis=0)
        max_bbox = max( [ (max_x[0]-min_x[0])*.6, (max_x[1]-min_x[1])*.6, (max_x[2]-min_x[2])*.6] )
        #print min_x
                                 
        #bbox = [(min(a), max(a)) for a in zip(bodies[self.currentIndex].collision.dofs.position)]
        #print bodies[self.currentIndex].collision.dofs.position
        
        #glDepthMask(GL_FALSE)
        #glDisable(GL_DEPTH_TEST)
        #glDepthFunc(GL_ALWAYS)

        glLineWidth( 1 )
        
        glDisable(GL_LIGHTING)
        glEnable(GL_COLOR_MATERIAL)
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glMultMatrixf( m.transpose() )
        glScalef( max_bbox, max_bbox, max_bbox)
        
        glColor3f(0,0,1)
        glutWireTorus(0.01,1,1,30)
        glBegin(GL_LINES)
        glVertex3f(0,0,-max_bbox*4)
        glVertex3f(0,0,max_bbox*4)
        glEnd()

        
        glColor3f(1,0,0)
        glRotatef( 90,0,1,0)
        glutWireTorus(0.01,1,1,30)
        glBegin(GL_LINES)
        glVertex3f(0,0,-max_bbox*4)
        glVertex3f(0,0,max_bbox*4)
        glEnd()
        
        
        glColor3f(0,1,0)
        glRotatef( 90,1,0,0)
        glutWireTorus(0.01,1,1,30)
        glBegin(GL_LINES)
        glVertex3f(0,0,-max_bbox*4)
        glVertex3f(0,0,max_bbox*4)
        glEnd()
        
        glPopMatrix()
        
        
        #glDepthFunc(GL_LESS)
        #glEnable(GL_DEPTH_TEST)
        #glDepthMask(GL_TRUE)
        
        
        
  
class SmilingCurveController(Sofa.PythonScriptController) :
    
    node = None
    currentIndex = 0
    points = []
            
    def initGraph(self, node):
        self.node = node

        for i in xrange(4):
            path = "P"+str(SMILING_CURVE_POINTS[i][0])+"/collision/dofs"
            self.points.append( ( path, SMILING_CURVE_POINTS[i][1] ) )

        print self.points
        sys.stdout.flush


        
#    def helloWorld(self):
#        print "hello"
        
    
    def draw(self):
        
        glDisable(GL_LIGHTING)
        glEnable(GL_COLOR_MATERIAL)
        
        glPointSize( 20 )
        glLineWidth( 5 )
        glColor3f(1,1,1)
        
        pos=[]

        for i in xrange(4):
            pos.append(self.node.getObject( self.points[i][0] ).position[self.points[i][1]])
            pos.append(self.node.getObject( self.points[i][0] ).position[self.points[i][1]])




        glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, pos);
        glEnable(GL_MAP1_VERTEX_3);

        glBegin(GL_LINE_STRIP);
        for i in xrange(30):
            glEvalCoord1f(i/30.0);
        glEnd();

        glBegin(GL_POINTS)
        p = self.node.getObject( self.points[0][0] ).position[self.points[0][1]]
        glVertex3f(p[0],p[1],p[2])
        p = self.node.getObject( self.points[1][0] ).position[self.points[1][1]]
        glVertex3f(p[0],p[1],p[2])
        p = self.node.getObject( self.points[2][0] ).position[self.points[2][1]]
        glVertex3f(p[0],p[1],p[2])
        p = self.node.getObject( self.points[3][0] ).position[self.points[3][1]]
        glVertex3f(p[0],p[1],p[2])
        glEnd()




        return 0


import Sofa
import os
import sys
import SofaImage.Tools
__file = __file__.replace('\\', '/')  # windows compatible filename
currentdir = os.path.dirname(os.path.realpath(__file__))+'/'
currentdir = currentdir.replace('\\', '/')
datadir = currentdir + '/data/'

global M0, M1
M0, M1 = None, None

def createScene(root):
    root.createObject('RequiredPlugin', pluginName='image')
    
    root.createObject('ImageContainer', name='image', filename='data/pelvis_f.raw')
    root.createObject('ImageFilter', name='engine', src='@image', param='50 50 5 200 200 50')
    root.createObject('ImageContainer', name='image2', inputImage='@engine.outputImage', inputTransform='@engine.outputTransform')
    root.createObject('ImageViewer', name='viewer', src='@image2', plane='84 53 15')
    #root.createObject('PythonScriptController', name="imageViewerScript", filename=__file, classname="PythonScript")
    root.createObject('OglModel', fileMesh='data/pelvis_f.obj', color='1 0.8 0.8 1')
    P0 = root.createChild('P0')
    P1 = root.createChild('P1')

    global M0, M1
    M0 = P0.createObject('MechanicalObject', template='Vec3', name='M0', showObject='1', showObjectScale='0.05', drawMode='2')
    M0.resize(0)
    M1 = P1.createObject('MechanicalObject', template='Vec3', name='M1', showObject='1', showObjectScale='0.05', drawMode='4')
    M1.resize(0)

# to control the interaction between the ImageViewer plane data widget and a python script controller, you must inherit from SofaImage.Tools.ImagePlaneController
class PythonScript(SofaImage.Tools.ImagePlaneController):
    points = {} # our dictionary of points: unique id as string (because we cannot pass a map with a non-string key to qml) -> point
    lastId = 0 # used to generate unique id
    mechanicalObject = None # the current mechanical object used to store new points

    def initGraph(self, root):
        self.useM0() # use the M0 mechanical object by default

    def useM0(self):
        global M0
        self.mechanicalObject = M0

    def useM1(self):
        global M1
        self.mechanicalObject = M1

    def generateId(self):
        id = self.lastId
        self.lastId += 1
        return id

    # override
    def addPoint(self, x, y, z):
        id = self.generateId()
        stringId = str(id)

        position = [x, y, z]

        global M1
        mechanicalObjectId = 0
        color = [1.0, 0.0, 0.0]
        if(M1 == self.mechanicalObject):
            mechanicalObjectId = 1
            color = [0.0, 0.0, 1.0]

        self.points[stringId] = {'mechanicalObjectId': mechanicalObjectId, 'positionIndex': len(self.mechanicalObject.position), 'position': position, 'color': color}
        self.mechanicalObject.position = self.mechanicalObject.position + [list([x, y, z])]
        self.mechanicalObject.resize(len(self.mechanicalObject.position))
        return id

    # override
    def removePoint(self, id):
        stringId = str(int(id))

        # point exists ?
        if(self.points[stringId]):
            mechanicalObjectId = self.points[stringId]['mechanicalObjectId']
            positionIndex = self.points[stringId]['positionIndex']

            # get the corresponding mechanical object
            mechanicalObject = None
            if(0 == mechanicalObjectId):
                global M0
                mechanicalObject = M0
            else:
                global M1
                mechanicalObject = M1

            # remove the point from the mechanical object
            mechanicalPosition = mechanicalObject.position
            del mechanicalPosition[positionIndex]
            mechanicalObject.position = mechanicalPosition
            mechanicalObject.resize(len(mechanicalObject.position))

            # shift the following position indices
            for key in self.points:
                if(self.points[key]['positionIndex'] > positionIndex):
                    self.points[key]['positionIndex'] = self.points[key]['positionIndex'] - 1

            # remove the point from our dictionary
            del self.points[stringId]

            return True

        return False

    # override
    def clearPoints(self):
        # clear our dictionary
        self.points = {}

        # clear the mechanical objects
        M0.resize(0)
        M1.resize(0)
        return

    # override
    def getPoints(self):
        return self.points

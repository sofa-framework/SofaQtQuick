import Sofa
import os
import sys
__file = __file__.replace('\\', '/')  # windows compatible filename
currentdir = os.path.dirname(os.path.realpath(__file__))+'/'
currentdir = currentdir.replace('\\', '/')
datadir = currentdir + '/data/'

global imageViewer
imageViewer = None

def createScene(root):
    root.createObject('RequiredPlugin', pluginName='image')
    
    root.createObject('ImageContainer', name='image', filename='data/pelvis_f.raw')
    root.createObject('ImageFilter', name='engine', src='@image', param='50 50 5 200 200 50')
    root.createObject('ImageContainer', name='image2', inputImage='@engine.outputImage', inputTransform='@engine.outputTransform')
    global imageViewer
    imageViewer = root.createObject('ImageViewer', name='viewer', src='@image2', plane='84 53 15')
    root.createObject('PythonScriptController', name="imageViewerScript", filename=__file, classname="PythonScript")
    root.createObject('OglModel', fileMesh='data/pelvis_f.obj', color='1 0.8 0.8 1')

class ImagePlaneController(Sofa.PythonScriptController):
    class Point(object):
        def __init__(self, position, color):
            self.position = position
            self.color = color

    #####

    def addPoint(self, id, x, y, z):
        return

    def removePoint(self, id):
        return

    def clearPoints(self):
        return

    def getPoints(self):
        return

class PythonScript(ImagePlaneController):
    node = None
    points = {}
    lastId = 0

    def createGraph(self, root):
        #print "image viewer script created"
        sys.stdout.flush()
        self.node = root

    def addPoint(self, x, y, z):
        id = self.lastId
        self.lastId += 1
        #print "add", str(id), str(x), str(y), str(z)
        sys.stdout.flush()
        position = {'x': x, 'y': y, 'z': z}
        color = {'r': 1.0, 'g': 1.0, 'b': 0.0}
        self.points[str(id)] = ImagePlaneController.Point(position, color)
        return id

    def removePoint(self, id):
        global imageViewer
        #print "remove", str(id)
        sys.stdout.flush()
        stringId = str(int(id))
        if(self.points[stringId]):
            del self.points[str(int(id))]
            return True

        return False

    def clearPoints(self):
        self.points = {}
        return

    def getPoints(self):
        return self.points

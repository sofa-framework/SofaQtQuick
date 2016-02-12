import Sofa

def createScene(root):
    root.createObject('PythonScriptController', name="script", filename=__file__, classname="PythonScript")
    root.createObject('VisualStyle',name='VisualStyle', displayFlags='showAll')

    node0 = root.createChild('0')
    node0.createObject('VisualModel', name='Visual Tagged 0', tags='0', fileMesh='mesh/cube.obj', translation='-2.0 0.0 0.0', material='Default Diffuse 1 1.0 0.0 0.0 1 Ambient 1 0.2 0.2 0.2 1 Specular 0 1 1 1 1 Emissive 0 0 0 0 0 Shininess 0 45')

    global mechanicalObject0
    mechanicalObject0 = node0.createObject('MechanicalObject', name='MechanicalObject', position='-2.0 0.0 2.0', showObject='1', drawMode='2')

    node1 = root.createChild('1')
    node1.createObject('VisualModel', name='Visual Tagged 1', tags='1', fileMesh='mesh/cube.obj', translation=' 2.0 0.0 0.0', material='Default Diffuse 1 0.0 0.0 1.0 1 Ambient 1 0.2 0.2 0.2 1 Specular 0 1 1 1 1 Emissive 0 0 0 0 0 Shininess 0 45')

    global mechanicalObject1
    mechanicalObject1 = node1.createObject('MechanicalObject', name='MechanicalObject', position=' 2.0 0.0 2.0', showObject='1', drawMode='4')

class PythonScript(Sofa.PythonScriptController) :
    node = None
    def createGraph(self, root):
        self.node = root

    def addParticle(self, position, tag):
        if(str(0) == str(tag)):
            global mechanicalObject0
            mechanicalObject0.position = mechanicalObject0.position + [list(position)]
            mechanicalObject0.resize(len(mechanicalObject0.position))
        if(str(1) == str(tag)):
            global mechanicalObject1
            mechanicalObject1.position = mechanicalObject1.position + [list(position)]
            mechanicalObject1.resize(len(mechanicalObject1.position))

    def removeParticle(self, index, tag):
        if(str(0) == str(tag)):
            global mechanicalObject0
            position = mechanicalObject0.position
            del position[index]
            mechanicalObject0.position = position
            mechanicalObject0.resize(len(mechanicalObject0.position))
        if(str(1) == str(tag)):
            global mechanicalObject1
            position = mechanicalObject1.position
            del position[index]
            mechanicalObject1.position = position
            mechanicalObject1.resize(len(mechanicalObject1.position))

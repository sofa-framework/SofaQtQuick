import Sofa

def createScene(root):
    root.createObject("MeshObjLoader", name="loader", filename="mesh/Armadillo_simplified.obj", triangulate="1")
    root.createObject("VisualModel", name="visual", src="@loader")
    root.createObject('ClipPlane', name="clipPlane", normal="1 0 0", position="0 0 0", active=False)
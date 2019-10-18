import Sofa
import sys

def createScene(node):
    print("CreateScene!")
    print(sys.argv[0])
    print(sys.argv[1])
    print(sys.argv[2])
    
    loader = node.addObject(sys.argv[1], name="loader", filename=sys.argv[2])
    vmodel = node.addObject("OglModel", name="vmodel", src=loader.getLinkPath())
    return node

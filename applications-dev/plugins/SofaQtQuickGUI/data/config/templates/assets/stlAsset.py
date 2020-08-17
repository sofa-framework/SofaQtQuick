#!/usr/bin/python3

import Sofa.Core
import subprocess
import os

# type_string: short asset description string
type_string = 'Triangular surface mesh'

# icon_path: url of the asset icon
icon_path = 'qrc:/icon/ICON_MESH.png'

# Used for Python scripts, determines whether it is sofa content or not
is_sofa_content = False

# Method called to instantiate the asset in the scene graph.
def create(node, assetName, assetPath):
    node.addObject('MeshSTLLoader', name='loader', filename=assetPath)
    node.addObject('OglModel', name=assetName, src='@loader')


# Method called to open a third party tool when clicking on 'Open in Editor'
def openThirdParty(assetPath):
    out = subprocess.Popen(['/snap/bin/blender', '--python',
                            os.path.dirname(os.path.realpath(__file__)) + '/openInBlender.py',
                            '--', 'stl', assetPath],
                           stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT)


def getInfo(tag, assetPath):
    out = subprocess.Popen(['/usr/bin/assimp', 'info', assetPath],
                           stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT)
    infolist = str(out.stdout.read()).split('\\n')
    for entry in infolist:
        if entry.startswith(tag):
            return entry.split(": ")[1]
    return "unknown"

def vertices(assetPath):
    return getInfo("Vertices", assetPath)

def faces(assetPath):
    return getInfo("Faces", assetPath)

def materials(assetPath):
    return getInfo("Materials", assetPath)

def meshes(assetPath):
    return getInfo("Meshes", assetPath)

def primitiveType(assetPath):
    return getInfo("Primitive Type", assetPath)

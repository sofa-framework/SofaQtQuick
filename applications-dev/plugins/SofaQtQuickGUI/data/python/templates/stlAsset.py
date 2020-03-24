#!/usr/bin/python3

import Sofa.Core
import subprocess
import os

type_string = "Triangular surface mesh"

icon_path = "/home/bruno/dev/similar/plugins/SofaQtQuick/applications-dev/plugins/SofaQtQuickGUI/data/icon/ICON_MESH.png"

is_sofa_content = False

def create(node, assetName, assetPath):
    node.addObject("MeshSTLLoader", name="loader", filename=assetPath)
    node.addObject("OglModel", name=assetName, src="@loader")


def getAssimpOutput(assetPath, token):
    out = subprocess.Popen(['/usr/bin/assimp', 'info', assetPath], 
                           stdout=subprocess.PIPE, 
                           stderr=subprocess.STDOUT)
    return str(out.stdout.read()).split(token)[1].split("\\n")[0].replace(" ", "")


def vertices(assetPath):
    return getAssimpOutput(assetPath, "Vertices:")

def faces(assetPath):
    return getAssimpOutput(assetPath, "Faces:")

def materials(assetPath):
    return getAssimpOutput(assetPath, "Materials:")

def meshes(assetPath):
    return getAssimpOutput(assetPath, "Meshes:")

def primitiveType(assetPath):
    return getAssimpOutput(assetPath, "Primitive Types:")

def openThirdParty(assetPath):
    out = subprocess.Popen(['/snap/bin/blender', '--python',
                            os.path.dirname(os.path.realpath(__file__)) + "/openInBlender.py",
                            "--", "stl", assetPath], 
                           stdout=subprocess.PIPE, 
                           stderr=subprocess.STDOUT)
    print(str(out.stdout.read()).replace("\\n", "\n"))

#!/usr/bin/python3

import Sofa.Core
import subprocess
import os

# type_string: short asset description string
type_string = 'QML CameraView Canvas'

# icon_path: url of the asset icon
icon_path = 'qrc:/icon/ICON_CANVAS.png'

# Used for Python scripts, determines whether it is sofa content or not
is_sofa_content = False

# Method called to instantiate the asset in the scene graph.
def create(node, assetName, assetPath):
    node.name = assetName
    node.addObject("Canvas", name=assetName, filename=assetPath)
    pass

# Method called to open a third party tool when clicking on 'Open in Editor'. Opens in favorite file editor by default
#def openThirdParty(assetPath):
#    pass

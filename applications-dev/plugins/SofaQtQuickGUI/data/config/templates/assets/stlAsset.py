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
    print(str(out.stdout.read()).replace('\n', ''))                                                                      

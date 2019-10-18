# This Python file uses the following encoding: utf-8

import os
import sys
import inspect
import importlib
import Sofa.Core
import splib

######################################################################
#################### INTROSPECTING HELPER METHODS ####################
######################################################################

def isScriptContent(m, obj):
    if inspect.isbuiltin(obj) or not callable(obj):
        return False
    if inspect.getmodule(obj).__file__ != m.__file__:
        return False
    return True


def collectMetaData(obj):
    data = {}
    data["name"] = obj.__name__
    if inspect.isclass(obj):
        data["type"] = "Controller" if issubclass(obj, Sofa.Core.Controller) else "DataEngine" if issubclass(obj, Sofa.Core.DataEngine) else "Class"

    else:
        data["type"] = "SofaScene" if obj.__name__ is "createScene" else "SofaPrefab" if type(obj) == splib.SofaPrefab else "Function"

    if data["type"] == "Class" or data["type"] == "Function":
        return None
    data["params"] = inspect.getfullargspec(obj.__wrapped__ if "__wrapped__" in dir(obj) else obj)
    data["sourcecode"] = inspect.getsource(obj)
    data["docstring"] = obj.__doc__ if obj.__doc__ != None else ""
    return data


# returns a dictionary of all callable SOFA objects in the module (Prefabs & createScenes),
# with their type as key
def getPythonScriptContent(moduledir, modulename):
    objects = {}
    # First let's load that script:
    try:
        sys.path.append(moduledir)

        if modulename in sys.modules:
            del(sys.modules[modulename])
        m = importlib.import_module(modulename)
    except ImportError as e:
        print ("PythonAsset ERROR: could not import module " + modulename)
        print (e)
        return objects
    except Exception as e:
        print ("Exception: in " + modulename + ":\n" + str(e))
        return objects

    for i in dir(m):
        obj = getattr(m, i)
        if isScriptContent(m, obj):
            meta = collectMetaData(obj)
            if meta != None:
                objects[i] = meta
    return objects


import code
def getPythonModuleDocstring(mpath):
    "Get module-level docstring of Python module at mpath, e.g. 'path/to/file.py'."
    print("LOADING THE DOCSTRING FROM: "+mpath)
    co = compile(open(mpath).read(), mpath, 'exec')
    if co.co_consts and isinstance(co.co_consts[0], basestring):
        docstring = co.co_consts[0]
    else:
        docstring = ""
    return str(docstring)



######################################################################
#################### SCENE GRAPH / PREFABS SAVING ####################
######################################################################


def getAbsPythonCallPath(node, rootNode):
    if rootNode.getPathName() == "/":
        return "root" + node.getPathName().replace("/", ".")[1:]
    else:
        # example:
        # rootNode.getPathName() = "/Snake/physics"
        # node.getPathName() = "/Snake/physics/visu/eye"
        # relPath = physics.visu.eye
        return rootNode.name.value + node.getPathName().replace(rootNode.getPathName(), "").replace("/", ".")

def buildDataParams(datas, indent, scn):
    s = ""
    for data in datas:
        if data.hasParent():
            scn[0] += indent + "### THERE WAS A LINK. "
            scn[0] += data.getParent().getLinkPath() + "=>" + data.getLinkPath() + "\n"
            if data.getName() != "name" and data.isPersistent():
                s += ", " + data.getName()+ "=" + repr(data.getParent().getLinkPath())
        else:
            if data.getName() != "name" and data.isPersistent():
                if " " not in data.getName() and data.getName() != "Help":
                    if data.getName() != "modulepath":
                        if data.getName() != "depend":
                            s += ", " + data.getName() + "=" + repr(data.value)
    return s

def saveRec(node, indent, modules, modulepaths, scn, rootNode):
    for o in node.objects:
        s = buildDataParams(o.getDataFields(), indent, scn)
        scn[0] += indent + getAbsPythonCallPath(node, rootNode) + ".addObject('"
        scn[0] += o.getClassName() + "', name='" + o.name.value + "'" + s + ")\n"

    for child in node.children:
        s = buildDataParams(child.getDataFields(), indent, scn)
        if child.getData("Prefab type") is not None:
            print('createPrefab')
            scn[0] += (indent + "####################### Prefab: " +
                       child.name + " #########################\n")
            scn[0] += (indent + child.getData("Prefab type").value +
                       "(" + getAbsPythonCallPath(node, rootNode) +
                       ".addChild('" + child.name.value + "'))\n")
            scn[0] += ("\n")
            modules.append(child.getData("Defined in").value)
            modulepaths.append(child.getData("modulepath").value)
        else:
            scn[0] += (indent + "####################### Node: " + child.name.value +
                       " #########################\n")
            scn[0] += (indent + getAbsPythonCallPath(node, rootNode) +
                       ".addChild('" + child.name.value + "')\n")
            saveRec(child, indent, modules, modulepaths, scn, rootNode)
            scn[0] += ("\n")


def getRelPath(path, relativeTo):
    # example:
    # path =       /home/bruno/dev/myproject/scripts
    # relativeTo = /home/bruno/dev/myproject/scenes/MyCoolFile.py
    # return "../scripts/"
    if os.path.isfile(relativeTo):
        relativeTo = os.path.dirname(relativeTo)

    if path == relativeTo:
        return ""
    commonPrefix = os.path.commonprefix([path, relativeTo])
    path = path.replace(commonPrefix, "")  # /scripts
    if path.startswith('/'):
        path = path[1:]  # scripts
    relativeTo = relativeTo.replace(commonPrefix, "")  # scenes
    newPath = ""
    if relativeTo != "":
        newPath += "../"
        for d in relativeTo.split('/'):
            newPath += "../"

    newPath += path
    return newPath


def saveAsPythonScene(fileName, node):
        root = node
        fd = open(fileName, "w+")

        fd.write('""" type: SofaContent """\n')
        fd.write("import sys\n")
        fd.write("import os\n")

        modules = []
        modulepaths = []
        scn = [""]
        saveRec(root, "\t", modules, modulepaths, scn, root)

        fd.write("# all Paths\n")
        for p in list(dict.fromkeys(modulepaths)):
            if p != "":
                fd.write("sys.path.append('" + getRelPath(p, fileName) +
                         "')\n")

        fd.write('# all Modules:\n')
        for m in list(dict.fromkeys(modules)):
            fd.write("from " + m + " import *\n")

        fd.write("\n\ndef createScene(root):\n")
        fd.write(scn[0])
        return True


def createPrefabFromNode(fileName, node, name, help):
#    try:
        print('Saving prefab')
        fd = open(fileName, "w+")
        fd.write("import sys\n")
        fd.write("import os\n")

        modules = []
        modulepaths = []
        scn = [""]
        saveRec(node, "\t\t", modules, modulepaths, scn, node)

        fd.write("# all Paths\n")
        for p in list(dict.fromkeys(modulepaths)):
            fd.write("sys.path.append('" + getRelPath(p, fileName) + "')\n")

        fd.write('# all Modules:\n')
        for m in list(dict.fromkeys(modules)):
            fd.write("from " + m + " import *\n")

        fd.write("\n\n@SofaPrefab\n")
        fd.write("class " + name + "():\n")
        fd.write("\t\"\"\" " + help + " \"\"\"\n")
        fd.write("\tdef __init__(self, " + node.name.value + "):\n")
        fd.write("\t\tself.node = " + node.name.value + "\n")
        fd.write(scn[0])
        return True
#    except Exception as e:
#        Sofa.msg_error(e)
#        return False


def loadMeshAsset(type, path, node):
    loader = node.addObject(type, name="loader", filename=path)
    vmodel = node.addObject("OglModel", name="vmodel", src=loader.getLinkPath())
    return node

def getPrefabMetaData(func, node):
    """ Retreives all meta data from a python function and stores it as datafields in the given node """
    inspect = importlib.import_module("inspect")
    f = func.__init__ if inspect.isclass(func) else func
    node.addData(name="modulename", value=inspect.getmodulename(f.__code__.co_filename), type="string", help="module containing the prefab", group="Infos")
    node.addData(name="prefabname", value=f.__code__.co_name, type="string", help="prefab's function name", group="Infos")
    node.addData(name="modulepath", value=f.__code__.co_filename, type="string", help="module path for this prefab", group="Infos")
    node.addData(name="lineno", value=f.__code__.co_firstlineno, type="int", help="first line number of the prefab in the module", group="Infos")
    node.addData(name="sourcecode", value=inspect.getsource(func), type="string", help="The prefab's source code", group="Infos")
    node.addData(name="docstring", value=func.__doc__, type="string", help="The prefab's docstring", group="Infos")
    return node




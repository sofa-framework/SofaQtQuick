# This Python file uses the following encoding: utf-8

import os
import sys
import inspect
import importlib
import Sofa.Core

######################################################################
#################### INTROSPECTING HELPER METHODS ####################
######################################################################

# returns a dictionary of all callable objects in the module, with their type as key
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

    # module loaded, let's see what's inside:
    if "createScene" in dir(m):
        print ("We've got a createScene entry point!")
        objects["createScene"] = { "type":"function", "docstring": m.createScene.__doc__ }
    for i in dir(m):
        if i == "SofaObject" or i == "SofaPrefab" or inspect.isbuiltin(i) or not callable(getattr(m, i)):
            continue
        class_ = getattr(m, i)
        if inspect.getmodule(class_).__file__ != m.__file__:
            continue

        docstring = str(class_.__doc__) if class_.__doc__ != None else "Undocumented prefab"
        if inspect.isclass(eval("m." + i)):
            if issubclass(eval("m." + i), Sofa.Core.Controller):
                objects[i] = { "type":"PythonScriptController", "docstring": docstring }
            elif issubclass(eval("m." + i), Sofa.Core.DataEngine):
                objects[i] = { "type":"PythonScriptDataEngine", "docstring": docstring }
            else:
                objects[i] = { "type":"Class", "docstring": docstring }
        else:
            if class_.__class__.__name__ == "SofaPrefab" \
               and i != "SofaPrefab":
                objects[i] = { "type":"SofaPrefab", "docstring": docstring }
            else:
                objects[i] = { "type":"function", "docstring": docstring }
    print(objects)
    return objects


import code
def getPythonModuleDocstring(mpath):
    "Get module-level docstring of Python module at mpath, e.g. 'path/to/file.py'."
    print(mpath)
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
        return rootNode.name + node.getPathName().replace(rootNode.getPathName(), "").replace("/", ".")

def buildDataParams(datas, indent, scn):
    s = ""
    for data in datas:
        if data.hasParent():
            scn[0] += indent + "### THERE WAS A LINK. "
            scn[0] += data.getParentPath() + "=>" + data.getLinkPath() + "\n"
            if data.name != "name" and data.isPersistant():
                s += ", " + data.name + "=" + repr(data.getParentPath())
        else:
            if data.name != "name" and data.isPersistant():
                if " " not in data.name and data.name != "Help":
                    if data.name != "modulepath":
                        if data.name != "depend":
                            s += ", " + data.name + "=" + repr(data.value)
    return s


def saveRec(node, indent, modules, modulepaths, scn, rootNode):
    for o in node.getObjects():
        s = buildDataParams(o.getListOfDataFields(), indent, scn)
        print('createObject')
        scn[0] += indent + getAbsPythonCallPath(node, rootNode) + ".createObject('"
        scn[0] += o.getClassName() + "', name='" + o.name + "'" + s + ")\n"

    for child in node.getChildren():
        s = buildDataParams(child.getListOfDataFields(), indent, scn)
        if child.getData("Prefab type") is not None:
            print('createPrefab')
            scn[0] += (indent + "####################### Prefab: " +
                       child.name + " #########################\n")
            scn[0] += (indent + child.getData("Prefab type").value +
                       "(" + getAbsPythonCallPath(node, rootNode) +
                       ".createChild('" + child.name + "'))\n")
            scn[0] += ("\n")
            modules.append(child.getData("Defined in").value)
            modulepaths.append(child.getData("modulepath").value)
        else:
            print("createNode")
            scn[0] += (indent + "####################### Node: " + child.name +
                       " #########################\n")
            scn[0] += (indent + getAbsPythonCallPath(node, rootNode) +
                       ".createChild('" + child.name + "')\n")
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
    try:
        root = node.getRootContext()
        fd = open(fileName, "w+")

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
    except Exception as e:
        Sofa.msg_error(e)
        return False


def createPrefabFromNode(fileName, node, name, help):
    try:
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
        fd.write("\tdef __init__(self, " + node.name + "):\n")
        fd.write("\t\tself.node = " + node.name + "\n")
        fd.write(scn[0])
        return True
    except Exception as e:
        Sofa.msg_error(e)
        return False


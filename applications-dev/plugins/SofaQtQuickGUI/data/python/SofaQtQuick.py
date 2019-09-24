# This Python file uses the following encoding: utf-8

import os
import sys
import inspect
import importlib
import Sofa.Core

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
    print(data["name"])
    if inspect.isclass(obj):
        func = obj.__init__
        data["type"] = "Controller" if issubclass(obj, Sofa.Core.Controller) else "DataEngine" if issubclass(obj, Sofa.Core.DataEngine) else "Class"

    else:
        func = obj
        data["type"] = "SofaScene" if obj.__name__ is "createScene" else "Function"

    data["lineno"] = func.__code__.co_firstlineno
    data["params"] = inspect.getfullargspec(func).args
    data["sourcecode"] = inspect.getsource(obj)
    data["docstring"] = obj.__doc__
    return data


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

    for i in dir(m):
        obj = getattr(m, i)
        if isScriptContent(m, obj):
            objects[i] = collectMetaData(obj)

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
        print('createObject')
        scn[0] += indent + getAbsPythonCallPath(node, rootNode) + ".createObject('"
        scn[0] += o.getClassName() + "', name='" + o.name.value + "'" + s + ")\n"

    for child in node.children:
        s = buildDataParams(child.getDataFields(), indent, scn)
        if child.getData("Prefab type") is not None:
            print('createPrefab')
            scn[0] += (indent + "####################### Prefab: " +
                       child.name + " #########################\n")
            scn[0] += (indent + child.getData("Prefab type").value +
                       "(" + getAbsPythonCallPath(node, rootNode) +
                       ".createChild('" + child.name.value + "'))\n")
            scn[0] += ("\n")
            modules.append(child.getData("Defined in").value)
            modulepaths.append(child.getData("modulepath").value)
        else:
            print("createNode")
            scn[0] += (indent + "####################### Node: " + child.name.value +
                       " #########################\n")
            scn[0] += (indent + getAbsPythonCallPath(node, rootNode) +
                       ".createChild('" + child.name.value + "')\n")
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
#    try:
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
#    except Exception as e:
#        Sofa.msg_error(e)
#        return False


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
    loader = node.createObject(type, name="loader", filename=path)
    vmodel = node.createObject("OglModel", name="vmodel", src=loader.getLinkPath())
    return node

def instantiatePrefab(func, node):
    inspect = importlib.import_module("inspect")
    f = func.__init__ if inspect.isclass(func) else func
    node.addData(name="modulename", value=inspect.getmodulename(f.__code__.co_filename), type="string", help="module containing the prefab", group="Infos")
    node.addData(name="prefabname", value=f.__code__.co_name, type="string", help="prefab's function name", group="Infos")
    node.addData(name="modulepath", value=f.__code__.co_filename, type="string", help="module path for this prefab", group="Infos")
    node.addData(name="lineno", value=f.__code__.co_firstlineno, type="int", help="first line number of the prefab in the module", group="Infos")
    node.addData(name="sourcecode", value=inspect.getsource(func), type="string", help="The prefab's source code", group="Infos")
    node.addData(name="docstring", value=func.__doc__, type="string", help="The prefab's docstring", group="Infos")

    node.addData(name="args", value=inspect.getfullargspec(f).args, type="vector<string>", help="Positional arguments", group="Infos")
    node.addData(name="varargs", value=inspect.getfullargspec(f).varargs, type="string", help="Uncollected positional arguments", group="Infos")
    node.addData(name="varkw", value=inspect.getfullargspec(f).varkw, type="string", help="Uncollected keyword arguments", group="Infos")
    node.addData(name="defaults", value=str(inspect.getfullargspec(f).defaults), type="string", help="Positional arguments defaults (serialized)", group="Infos")
    node.addData(name="kwonlyargs", value=inspect.getfullargspec(f).kwonlyargs, type="vector<string>", help="Keyword arguments", group="Infos")
    node.addData(name="kwonlydefaults", value=str(inspect.getfullargspec(f).kwonlydefaults), type="string", help="Keyword arguments defaults (serialized)", group="Infos")
    node.addData(name="annotations", value=str(inspect.getfullargspec(f).annotations), type="string", help="function's annotations (serialized)", group="Infos")
    func(node)


def loadPythonAsset(moduledir, modulename, prefabname, node):
#    print("#############   LOADING PYTHON ASSET")
#    print("#############   moduledir: " + moduledir)
#    print("#############   modulename: " + modulename)
#    print("#############   prefabname: " + prefabname)
#    print("#############   node.name: " + node.name)
    functions = {}
    pythonScripts = {}
    classes = {}
    prefabs = {}

    # First let's load that script
    sys.path.append(moduledir)
    m = importlib.import_module(modulename)

    # module loaded, let's see what's inside:
    if "createScene" in dir(m):
        # print("We found a createScene entry point, let's load it")
        functions["createScene"] = m.createScene
    for i in dir(m):
        if i == "SofaObject" or i == "SofaPrefab":
            continue
        if inspect.isclass(eval("m." + i)):
            # A non-decorated class
            if issubclass(eval("m." + i), Sofa.Core.Controller) or \
               issubclass(eval("m." + i), Sofa.Core.DataEngine):
                pythonScripts[i] = getattr(m, i)
            else:
                classes[i] = getattr(m, i)
        else:
            class_ = getattr(m, i)
            # a class decorated with @SofaPrefab:
            if class_.__class__.__name__ == "SofaPrefab" \
               and i != "SofaPrefab":
                prefabs[i] = class_
            elif callable(class_) and not inspect.isbuiltin(i):
                functions[i] = class_

    # No Prefab name passed as argument. Loading 1 prefab:
    if "" == prefabname:
        print ('No prefab name provided. Loading by order of priority')
        # 1st, createScene if it exists:
        if 'createScene' in functions.keys():
            functions['createScene'](node)
            return node
        # 2nd, call the first prefab available:
        if len(prefabs) > 0:
            instantiatePrefab(prefabs.items()[0][1], node)
            return node
        # 3rd, call the first PythonScript:
        if len(pythonScripts) > 0:
            instantiatePrefab(pythonScripts.items()[0][1], node)
            return node
        # 4th, call the first class:
        if len(classes) > 0:
            instantiatePrefab(classes.items()[0][1], node)
            return node
        # finally, try to call the first python function available:
        if len(functions) > 0:
            instantiatePrefab(functions.items()[0][1], node)
            return node
        print ("PythonAsset ERROR: No callable found in " + str(m))
    else:
        pyName = prefabname
        print("#############" + prefabname)
        if pyName == "createScene":
            print('Loading ' + pyName)
            functions["createScene"](node)
        elif pyName in prefabs.keys():
            print('Loading ' + pyName)
            instantiatePrefab(prefabs[pyName], node)
        elif pyName in pythonScripts.keys():
            print('Loading ' + pyName)
            instantiatePrefab(pythonScripts[pyName], node)
        elif pyName in classes.keys():
            print('Loading ' + pyName)
            instantiatePrefab(classes[pyName], node)
        elif pyName in functions.keys():
            print('Loading ' + pyName)
            instantiatePrefab(functions[pyName], node)
        else:
            print ("PythonAsset ERROR: No callable object with name " + pyName)
    return node



# This Python file uses the following encoding: utf-8

import os
import sys
import inspect
import importlib
import importlib.util
import code
import Sofa
import Sofa.Core
import Sofa.Helper
import SofaApplication
import splib

######################################################################
#################### INTROSPECTING HELPER METHODS ####################
######################################################################

def isScriptContent(m, obj):
    if inspect.isbuiltin(obj) or not callable(obj):
        return False
    return True


def collectMetaData(obj):
    data = {}
    data["name"] = obj.__name__
    if inspect.isclass(obj):
        if issubclass(obj, Sofa.Core.Controller):
            data["type"] = "Controller"
        elif issubclass(obj, Sofa.Core.DataEngine):
            data["type"] = "DataEngine"
        elif issubclass(obj, Sofa.Core.ForceField):
            data["type"] = "ForceField"
        elif issubclass(obj, Sofa.Core.RawPrefab):
            data["type"] = "SofaPrefab"
        else:
            data["type"] = "Class"
    elif obj.__name__ is "createScene":
        data["type"] = "SofaScene"
    elif obj.__name__ is "SofaPrefabF" or type(obj) == Sofa.PrefabBuilder:
        data["type"] = "SofaPrefab"
    else:
        data["type"] = "Function"

    ## Class and Function are not callabled from the GUI. Unless having the callable decorator
    if data["type"] == "Class" or data["type"] == "Function":
        return None

    data["params"] = inspect.getfullargspec(obj.__original__ if "__original__" in dir(obj) else obj)
    data["sourcecode"] = inspect.getsource(obj.__original__ if "__original__" in dir(obj) else obj)
    data["docstring"] = obj.__original__.__doc__ if "__original__" in dir(obj) else obj.__doc__ if obj.__doc__ != None else ""
    data["lineno"] = str(inspect.findsource(obj.__original__ if "__original__" in dir(obj) else obj)[1])
    return data

# returns a dictionary of all callable objects in the module, with their type as key
def getPythonModuleContent(moduledir, modulename):
    objects = {}

    # First let's load that script:
    spec = importlib.util.spec_from_file_location(modulename, moduledir+"/"+modulename)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    sys.modules[modulename] = m

    m.__file__=moduledir+"/"+modulename

    for i in dir(m):
        obj = getattr(m, i)
        if isScriptContent(m, obj):
            meta = collectMetaData(obj)
            if meta != None:
                objects[i] = meta
    return objects

def getPythonModuleDocstring(mpath):
    "Get module-level docstring of Python module at mpath, e.g. 'path/to/file.py'."
    co = compile(open(mpath).read(), mpath, 'exec')
    if co.co_consts and isinstance(co.co_consts[0], str):
        docstring = co.co_consts[0]
    else:
        docstring = ""
    return str(docstring)


######################################################################
#################### SCENE GRAPH / PREFABS SAVING ####################
######################################################################
def getAbsPythonCallPath(node, rootNode):
    if rootNode.getPathName() == "/":
        if node.getPathName() == "/":
            return rootNode.name.value
        return rootNode.name.value + node.getPathName().replace("/", ".")
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
            if data.getName() != "name":
                relPath = os.path.relpath(data.getParent().getPathName(), data.getOwner().getContext().getPathName())
                s += ", " + data.getName()+ "='@" + relPath +"'"
        else:
            if data.getName() not in ["name","prefabname", "docstring"] and (data.isPersistent() or data.isRequired()):
                if " " not in data.getName() and data.getName() != "Help":
                    if data.getName() != "modulepath":
                        if data.getName() != "depend":
                            v = repr(data.value)
                            if "DataFileName" in str(data):
                                if os.path.exists(os.path.dirname(SofaApplication.getScene()) + "/" + v[1:-1]):
                                    # If the path exists & is relative
                                    s += ", " + data.getName() + "='" + v[1:-1] + "'"
                                elif os.path.relpath(v[1:-1], os.path.dirname(SofaApplication.getScene())):
                                    # If the path can be expressed relative to the scene file:
                                    s += ", " + data.getName() + "='" + os.path.relpath(v[1:-1], os.path.dirname(SofaApplication.getScene())) + "'"
                                else:
                                    # fallback (relative path, but not relative to scene file)
                                    s += ", " + data.getName() + "='" + v[1:-1] + "'"
                            else:
                                s += ", " + data.getName() + "=" + ( v[v.find('['):v.rfind(']')+1] if "array" in repr(data.value) else repr(data.value))
    return s

def saveRec(node, indent, modules, modulepaths, scn, rootNode):
    for o in node.objects:
        s = buildDataParams(o.getDataFields(), indent, scn)
        scn[0] += indent + getAbsPythonCallPath(node, rootNode) + ".addObject('"
        scn[0] += o.getClassName() + "', name='" + o.name.value + "'" + s + ")\n"

    for child in node.children:
        s = buildDataParams(child.getDataFields(), indent, scn)
        if child.getData("prefabname") is not None:
            #print('createPrefab '+str(child.name))
            scn[0] += (indent + "####################### Prefab: " +
                       child.name.value + " #########################\n")
            scn[0] += (indent + getAbsPythonCallPath(node, rootNode) +
                       ".addChild(" + child.prefabname.value + "(name='"+ child.name.value +"'"+s+ "))\n")
            scn[0] += ("\n")
            dirname, filename = os.path.split(child.getDefinitionSourceFileName())
            modules.append(filename)
            modulepaths.append(dirname)
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
    saveRec(root, "    ", modules, modulepaths, scn, root)

    fd.write("# all Paths\n")
    for p in list(dict.fromkeys(modulepaths)):
        if p != "":
            fd.write("sys.path.append('" + getRelPath(p, fileName) +
                     "')\n")

    fd.write('# all Modules:\n')
    for m in list(dict.fromkeys(modules)):
        fd.write("from " + m + " import *\n")

    fd.write("\n\ndef createScene("+ node.getName() +"):\n")
    fd.write(scn[0])
    return True

def callFunction(file, function, *args, **kwargs):
    # First let's load that script:
    importlib.invalidate_caches()

    moduledir = os.path.dirname(file)
    modulename = os.path.basename(file)
    spec = importlib.util.spec_from_file_location(modulename, moduledir+"/"+modulename)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return getattr(m, function)(*args, **kwargs)


def createTypeConversionEngine(modulename, node, srcData, dstData, srcType, dstType):
    engineName = "to_"+dstData.getOwner().getName() + "_" + dstData.getName()
    if engineName not in node.objects:
        m = importlib.import_module(modulename)
        e = splib.TypeConversionEngine(name=engineName, dstType=dstData.typeName())
        dstData.setParent(e.dst)
        node.addObject(e)
    e.addDataConversion(srcData, m.convert)


def createPrefabFromNode(fileName, node, name, help):
    print('Saving prefab in ' + fileName)
    fd = open(fileName, "w+")
    fd.write('"""type: SofaContent"""\n')
    fd.write("import sys\n")
    fd.write("import os\n")
    fd.write("import Sofa\n")
    fd.write("import Sofa.Core\n")


    modules = []
    modulepaths = []
    scn = [""]
    nodeName = node.getName()
    print(node.name)
    node.setName("self")
    print(nodeName)
    saveRec(node, "        ", modules, modulepaths, scn, node)
    print("saved rec")
    node.setName(nodeName)
    print(node.name)

    fd.write("# all Paths\n")
    for p in list(dict.fromkeys(modulepaths)):
        fd.write("sys.path.append('" + getRelPath(p, fileName) + "')\n")

    fd.write('# all Modules:\n')
    for m in list(dict.fromkeys(modules)):
        fd.write("from " + m + " import *\n")

    fd.write("class " + name + "(Sofa.Prefab):\n")
    fd.write("    \"\"\" " + help + " \"\"\"\n")
    fd.write("    def __init__(self, *args, **kwargs):\n")
    fd.write("        Sofa.Prefab.__init__(self, *args, **kwargs)\n")
    fd.write(scn[0])
    return True


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




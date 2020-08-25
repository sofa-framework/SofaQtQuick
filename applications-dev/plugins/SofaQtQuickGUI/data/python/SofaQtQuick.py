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
from graph_serialization import GraphSerializer

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


def createTypeConversionEngine(modulename, node, srcData, dstData, srcType, dstType):
    engineName = "to_"+dstData.getOwner().getName() + "_" + dstData.getName()
    if engineName not in node.objects:
        m = importlib.import_module(modulename)
        e = splib.TypeConversionEngine(name=engineName, dstType=dstData.typeName())
        dstData.setParent(e.dst)
        node.addObject(e)
    e.addDataConversion(srcData, m.convert)



######################################################################
#################### SCENE GRAPH / PREFABS SAVING ####################
#######################################################################

def saveAsPythonPrefab(fileName, prefab, name, help):
    serializer = GraphSerializer(fileName)
    serializer.save(prefab, name, help, True)


def saveAsPythonScene(fileName, node):
    serializer = GraphSerializer(fileName)
    serializer.save(node)



def callFunction(file, function, *args, **kwargs):
    # First let's load that script:
    importlib.invalidate_caches()

    moduledir = os.path.dirname(file)
    modulename = os.path.basename(file)
    spec = importlib.util.spec_from_file_location(modulename, moduledir+"/"+modulename)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return getattr(m, function)(*args, **kwargs)



def getPrefabMetaData(func, node):
    """ Retreives all meta data from a python function and stores it as datafields in the given node """
    inspect = importlib.import_module("inspect")
    f = func.__init__ if inspect.isclass(func) else func
    node.addData(name="modulename", value=inspect.getmodulename(f.__code__.co_filename), type="string", help="module containing the prefab", group="Infos")
    node.addData(name="prefabname", value=f.__code__.co_name, type="string", help="prefab's name", group="Infos")
    node.addData(name="modulepath", value=f.__code__.co_filename, type="string", help="module path for this prefab", group="Infos")
    node.addData(name="lineno", value=f.__code__.co_firstlineno, type="int", help="first line number of the prefab in the module", group="Infos")
    node.addData(name="sourcecode", value=inspect.getsource(func), type="string", help="The prefab's source code", group="Infos")
    node.addData(name="docstring", value=func.__doc__, type="string", help="The prefab's docstring", group="Infos")
    return node




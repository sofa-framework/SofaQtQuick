import Sofa
import sys
import importlib
import inspect

def createScene(node):
    functions = {}
    pythonScripts = {}
    classes = {}
    prefabs = {}
    # Let's see our arguments:
    if len(sys.argv) < 3:
        print ("PythonAsset ERROR: not enough arguments. Usage: "
               "PythonAsset.py <moduledir> <modulename> "
               "[PrefabName] [PrefabArguments ...]")

    # First let's load that script:
    try:
        sys.path.append(sys.argv[1])
        m = importlib.import_module(sys.argv[2])
    except ImportError, e:
        print ("PythonAsset ERROR: could not import module " + sys.argv[2])
        print (e)

    # module loaded, let's see what's inside:
    if "createScene" in dir(m):
        # print("We found a createScene entry point, let's load it")
        functions["createScene"] = m.createScene
    for i in dir(m):
        if i == "SofaObject" or i == "SofaPrefab":
            continue
        if inspect.isclass(eval("m." + i)):
            # A non-decorated class
            if issubclass(eval("m." + i), Sofa.PythonScriptController) or \
               issubclass(eval("m." + i), Sofa.PythonScriptDataEngine):
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
    if len(sys.argv) == 3:
        # print ('No prefab name provided. Loading by order of priority')
        # 1st, createScene if it exists:
        if 'createScene' in functions.keys():
            functions['createScene'](node)
            return node
        # 2nd, call the first prefab available:
        if len(prefabs) > 0:
            prefabs.items()[0][1](node)
            return node
        # 3rd, call the first PythonScript:
        if len(pythonScripts) > 0:
            pythonScripts.items()[0][1](node)
            return node
        # 4th, call the first class:
        if len(classes) > 0:
            classes.items()[0][1](node)
            return node
        # finally, try to call the first python function available:
        if len(functions) > 0:
            functions.items()[0][1](node)
            return
        print ("PythonAsset ERROR: No callable found in " + str(m))
    else:
        pyName = sys.argv[3]
        if pyName == "createScene":
            print 'Loading ' + pyName
            functions["createScene"](node)
        elif pyName in prefabs.keys():
            print 'Loading ' + pyName
            prefabs[pyName](node)
        elif pyName in pythonScripts.keys():
            print 'Loading ' + pyName
            pythonScripts[pyName](node)
        elif pyName in classes.keys():
            print 'Loading ' + pyName
            classes[pyName](node)
        elif pyName in functions.keys():
            print 'Loading ' + pyName
            functions[pyName](node)
        else:
            print ("PythonAsset ERROR: No callable object with name " + pyName)
    return node

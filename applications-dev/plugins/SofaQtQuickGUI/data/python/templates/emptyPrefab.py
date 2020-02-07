"""type: SofaContent"""

import Sofa

class %ComponentName%(Sofa.Prefab):
    def __init__(self, *args, **kwargs):
        Sofa.Prefab.__init__(self, *args, **kwargs)
        # ## Add here some prefab parameters if you want example:
        # self.addPrefabParameter(name="nSpheres", value=kwargs.get("nSpheres", 5), type="int", help="number of spheres to create")
        # self.addPrefabParameter(name="spacing", value=kwargs.get("spacing", 1.5), type="double", help="spacing between spheres")
        pass
    
    def doReInit(self, *args, **kwargs):
        # ## This function will be called everytime a prefab param changes. The prefab node's content will be erased & whatever is done in this function will be applied again:
        # for i in range(self.nSpheres.value):
        #     self.addObject("OglModel", name="sphere_"+str(i), filename="mesh/sphere.obj", translation=i * self.spacing.value)
        pass


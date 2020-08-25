import os
import Sofa.Core


class GraphSerializer:
    def __init__(self, filename):
        self.filename = filename


    def save(self, node, name = "", help = "", is_prefab = False):
        self.node = node
        self.name = name if name else node.getName()
        self.help = help
        self.is_prefab = is_prefab
        self.indent = "        " if self.is_prefab else "    "
        created = [self.node]
        not_created = []

        with open(self.filename, 'w') as fd:
            self.write_header(fd)
            self.write_import_statements(fd)

            self.get_external_dependencies()
            self.write_definition(fd)

            if self.is_prefab:
                nodeName = self.node.getName()
                self.node.setName("self")

                self.write_prefab_parameters(fd)

                fd.write("\n\n    def doReInit(self):\n")

                # self.add_external_deps_check(fd)
                createdDeps = [(n.getParent().getOwner() if issubclass(type(n), Sofa.Core.Data) else n) for n in self.external_deps]
                created = created + createdDeps
                self.createGraph(fd, created, not_created)

                self.node.setName(nodeName)
            else:
                self.createGraph(fd, created, not_created)


    # def add_external_deps_check(self, fd):
    #     if len(self.external_deps) == 0:
    #         return
    #     fd.write(self.indent + "if ")
    #     first = True
    #     for item in self.external_deps:
    #         if not first:
    #             fd.write(" or ")
    #         first = False
    #         itemName = self.getParameterName(item)
    #         fd.write("not self.findData('"+ itemName +"')")

    #     fd.write(':\n' + self.indent + 'return\n')



    def getParameterName(self, param):
        if isinstance(param, Sofa.Core.Data):
            parentLinkOwner = param.getParent().getOwner().getName()
            paramOwner = param.getOwner().getPathName().replace(self.node.getPathName(), "").replace("/", "_").replace(".", "_")[1:]
            return paramOwner + "_" + parentLinkOwner
        return param.getPathName().replace(self.node.getPathName(), "").replace("/", "_").replace(".", "_")[1:]


    def write_prefab_parameters(self, fd):
        fd.write("\n\n    \"\"\" Prefab parameters: \"\"\"\n")
        fd.write("    def createParams(self, *args, **kwargs):\n")
        fd.write(self.indent + "# external deps: ")
        if len(self.external_deps) == 0:
            fd.write("No external dependency\n")
            fd.write(self.indent + "pass")
        else:
            fd.write(", ".join((dep.getPathName().replace(self.node.getPathName(), "") if isinstance(dep, Sofa.Core.Data) else dep.getName()) for dep in self.external_deps) + "\n")
        str = ''
        extParamsList = []
        for item in self.external_deps:
            itemName = self.getParameterName(item)
            if itemName not in extParamsList:
                extParamsList.append(itemName)
                if isinstance(item, Sofa.Core.Data):
                    fd.write(self.indent + "self.addPrefabParameter(name='" + itemName + "', type='Link', help='', default=kwargs.get('"+itemName+"', None))\n")
                else:
                    fd.write(self.indent + "self.addPrefabParameter(name='" + itemName + "', type='Link', help='" + item.getHelp()  + "', default=kwargs.get('"+itemName+"', None))\n")


    def write_definition(self, fd):
        if self.is_prefab:
            fd.write("\n\nclass " + self.name + "(Sofa.Prefab):\n")
            fd.write("    \"\"\" " + self.help + " \"\"\"\n")
            fd.write("    def __init__(self, *args, **kwargs):\n")
            fd.write("        Sofa.Prefab.__init__(self, *args, **kwargs)\n")
        else:
            fd.write("\n\ndef createScene(root):\n")
            fd.write('    """ ' + self.help + ' """\n')


    def get_external_dependencies(self):
        self.external_deps = []
        """ writes into self.external_deps all dependencies to potential objects external to self.node """
        def getExternalDependencyFromItem(item):
            for d in item.getDataFields():
                if d.hasParent() and not d.getParent().getPathName().startswith(self.node.getPathName()):
                    self.external_deps.append(d)
            for l in item.getLinks():
                if l.getLinkedBase() is not None and not l.getLinkedBase().getPathName().startswith(self.node.getPathName()) and l.getName() != "parents":
                    self.external_deps.append(l)

        def r_getExternalDependencies(currentNode):
            getExternalDependencyFromItem(currentNode)

            for o in currentNode.objects:
                getExternalDependencyFromItem(o)

            for c in currentNode.children:
                r_getExternalDependencies(c)

        r_getExternalDependencies(self.node)


    def write_header(self, fd):
        fd.write('"""type: SofaContent"""\n')


    def getRelPath(self, path, relativeTo):
        """
        returns `path`, expressed relatively to `relativeTo`. For instance:
        path =       /home/bruno/dev/myproject/scripts
        relativeTo = /home/bruno/dev/myproject/scenes/MyCoolFile.py
        return "../scripts/"
        """
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


    def write_import_statements(self, fd):
        """
        crawls self.node looking for prefabs, store the module names corresponding to
        the definition files of those prefabs, and finally write down both the import statements
        and adding of those modules' directories to sys.path
        """
        fd.write("import sys\n")
        fd.write("import os\n")
        fd.write("import Sofa.Core\n")

        modules = []
        module_paths = []
        def r_write_imports(node):
            for child in node.children:
                if child.getData("prefabname") is not None:
                    dirname, filename = os.path.split(child.getDefinitionSourceFileName())
                    modules.append(filename)
                    module_paths.append(dirname)
                else:
                    r_write_imports(child)

        r_write_imports(self.node)
        if len(module_paths) != 0:
            fd.write("# all paths:\n")
        for p in module_paths:
            fd.write("sys.path.append('" + self.getRelPath(p, self.filename) + "')\n")

        if len(modules) != 0:
            fd.write("# all modules:\n")
        for m in modules:
            fd.write("from " + m + " import *\n")


    def get_rel_path(self, src, dest):
        # relpath is calculated from contexts. if src is a component, get his context
        return os.path.relpath(dest.getPathName(), src.getContext().getPathName() if hasattr(src, "getContext") else src.getPathName())


    def buildDataParams(self, fd, obj):
        """
        takes an object (node, component, prefab...) and write down
        the list of attributes (datafields, links, template type) of this object into the returned string.
        This string can then be used (in createItem for instance)
        to serialize a component using the python syntax
        """
        s = ""
        datas = obj.getDataFields()
        links = obj.getLinks()
        for data in datas:
            if data.hasParent():
                if self.is_prefab and data in self.external_deps:
                    fd.write(self.indent + "### THERE WAS A LINK. ")
                    if data.getName() != "name":
                        fd.write(data.getParent().getLinkPath() + " => " + data.getParent().getLinkPath() + "\n")
                        relPath = os.path.relpath(data.getParent().getPathName(), data.getOwner().getContext().getPathName())
                        # s += ", " + data.getName()+ "='@" + relPath +"'"
                        s += ", " + data.getName() + "=self." + self.getParameterName(data) + "." + data.getName()
                else:
                    if data.getName() != "name":
                        fd.write(self.indent + "### THERE WAS A LINK. ")
                        fd.write(obj.getPathName() + "." + data.getName() + " => " + data.getParent().getLinkPath() + "\n")
                        relPath = self.get_rel_path(obj, data.getParent().getOwner()) + "." + data.getParent().getName()
                        s += ", " + data.getName()+ "='@" + relPath + "'"
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
                                elif "DataLink" in str(data):
                                    s += ", " + data.getName() + "='" + v + "'"
                                else:
                                    s += ", " + data.getName() + "=" + ( v[v.find('['):v.rfind(']')+1] if "array" in v else v)
        for link in links:
            if link in self.external_deps and self.is_prefab:
                s += ", " + link.getName() + "=" + "self." + self.getParameterName(link) + ".value"

            elif link.getLinkedBase() and link.getName() != "context" and link.getName() != "parents" and link.getName() != "child" and link.getName() != "object" and link.getName() != "state" and link.getName() != "visualModel" and link.getName() != "unsorted" and link.getName() != "mapping":
                s += ", " + link.getName() + "='" + link.getLinkedPath() + "'"

        entry = Sofa.Core.ObjectFactory.getComponent(obj.getClassName())
        if entry.defaultTemplate != obj.getTemplateName():
            s += ", template='" + obj.getTemplateName() + "'"
        return s


    def getAbsPythonCallPath(self, node):
        """ returns a string corresponding to the call sequence from the top node (e.g. self[Snake.physics.visu.eye])"""
        if self.node.getPathName() == node.getPathName():
            return self.node.name.value
        if self.node.getPathName() == "/":
            return self.node.name.value + "['" + node.getPathName().replace("/", ".")[1:] + "']"
        else:
            return self.node.name.value + "['" + node.getPathName().replace(self.node.getPathName(), "").replace("/", ".")[1:] + "']"


    def createItem(self, fd, item):
        if type(item) is Sofa.Core.Node:
            fd.write(self.indent + self.getAbsPythonCallPath(item.parents[0]) + ".addChild('" + item.getName() + "')\n")
        else:
            attributes = self.buildDataParams(fd, item)
            parentPath = self.getAbsPythonCallPath(item.getContext() if hasattr(item, "getContext") else item.parents[0])
            if isinstance(item, Sofa.Prefab):
                fd.write(self.indent + str(item.prefabname.value) + "(" + parentPath + ", name='" + item.getName() + "'" + attributes + ")\n")
            else:
                fd.write(self.indent + parentPath + ".addObject('" + item.getClassName() + "', name='" + item.getName() + "'" + attributes + ")\n")
        return item


    def getDependenciesFor(self, object):
        if type(object) is Sofa.Core.Node:
            return object.parents
        list = []
        for d in object.getDataFields():
            if d.getParent() and type(d.getParent().getOwner()) != Sofa.Core.Node and (self.is_prefab and not d.getParent().getPathName().startswith(self.node.getPathName())):
                list.append(d.getParent().getOwner())

        for l in object.getLinks():
            if l.getLinkedBase() and type(l.getLinkedBase()) != Sofa.Core.Node and l.getName() != "slaves":
                if not self.is_prefab or l.getLinkedPath().startswith(self.node.getPathName()):
                    list.append(l.getLinkedBase())

        list.append(object.getContext())
        return list


    def hasNotYetCreatedDependencies(self, item, created):
        for dep in self.getDependenciesFor(item):
            if dep not in created:
                return True
        return False


    def try_create_dependent_item(self, fd, item, created, not_created):
        for o in not_created:
            if not self.hasNotYetCreatedDependencies(o, created):
                created.append(self.createItem(fd, o))
                not_created.remove(o)


    def createGraph(self, fd, created, not_created):
        def r_createGraph(node):
            for o in node.objects:
                if not self.hasNotYetCreatedDependencies(o, created):
                    created.append(self.createItem(fd, o))
                else:
                    fd.write(self.indent + "# " + o.getName() + " (" + o.getClassName() + ") will be created later because of upstream dependencies\n")
                    not_created.append(o)

            # Deal with components having a dependency to upstream-located or sibling-located components
            self.try_create_dependent_item(fd, node, created, not_created)
            for c in node.children:
                fd.write("\n")
                created.append(self.createItem(fd, c))
                if not isinstance(c, Sofa.Prefab):
                    r_createGraph(c)
                # Deal with components having a dependency to upstream-located or sibling-located components
                self.try_create_dependent_item(fd, c, created, not_created)

        r_createGraph(self.node)




























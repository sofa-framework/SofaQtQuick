#include "PythonConsole.h"

#include <SofaPython/PythonEnvironment.h>
#include <SofaPython/SceneLoaderPY.h>

namespace sofa
{

namespace qtquick
{

using namespace sofa::simulation;

// PythonConsole Module Definition
namespace {
    static const std::string moduleName = "PythonConsole";

    static PyObject* Write(PyObject *self, PyObject *args)
    {
        const char *arg;

        if(!PyArg_ParseTuple(args, "s", &arg))
            return NULL;

        PythonConsole::AddText(arg);

        return Py_BuildValue("");
    }

    static PyMethodDef PythonConsoleMethods[] = {
        {"write", Write, METH_VARARGS, "Write text in the PythonConsole(s)"},
        {NULL, NULL, 0, NULL}
    };

    class RAII
    {
    public:
        RAII()
        {
            SceneLoaderPY::addModules(moduleName.c_str(), PythonConsoleMethods);
        }

        ~RAII()
        {
            SceneLoaderPY::removeModules(moduleName.c_str());
        }
    };

    static RAII singleton;
}

PythonConsole::PythonConsole(QObject *parent) : QObject(parent)
{
    OurPythonConsoles.insert(this);
}

PythonConsole::~PythonConsole()
{
    OurPythonConsoles.remove(this);
}

QSet<PythonConsole*> PythonConsole::OurPythonConsoles;

void PythonConsole::AddText(const QString& text)
{
    for(PythonConsole* pythonConsole : OurPythonConsoles)
        pythonConsole->textAdded(text);
}

}

}

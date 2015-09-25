#include "PythonConsole.h"

#include <SofaPython/PythonEnvironment.h>

#include <QTextStream>
#include <QFile>

namespace sofa
{

namespace qtquick
{

using namespace sofa::simulation;

// PythonConsole Module Definition
namespace PythonConsoleModule {
    static std::string Name() {return "PythonConsole";}

    static PyObject* Write(PyObject */*self*/, PyObject *args)
    {
        const char *arg;

        if(!PyArg_ParseTuple(args, "s", &arg))
            return NULL;

        PythonConsole::AddText(arg);

        return Py_BuildValue("");
    }

    static PyMethodDef Methods[] = {
        {"write", Write, METH_VARARGS, "Write text in the PythonConsole(s)"},
        {NULL, NULL, 0, NULL}
    };
}

PythonConsole::PythonConsole(QObject *parent) : QObject(parent)
{
    InitConsoleRedirection();

    OurPythonConsoles.insert(this);
}

PythonConsole::~PythonConsole()
{
    OurPythonConsoles.remove(this);
}

void PythonConsole::AddText(const QString& text)
{
    for(PythonConsole* pythonConsole : OurPythonConsoles)
        pythonConsole->textAdded(text);
}

QSet<PythonConsole*> PythonConsole::OurPythonConsoles;

void PythonConsole::InitConsoleRedirection()
{
    static bool redirect = false;
    if(!redirect)
    {
        PythonEnvironment::addModule(PythonConsoleModule::Name(), PythonConsoleModule::Methods);

        QFile pythonFile(":/python/PythonConsole.py");
        pythonFile.open(QIODevice::ReadOnly | QIODevice::Text);
        PythonEnvironment::runString(QTextStream(&pythonFile).readAll().toStdString());

        redirect = true;
    }
}

}

}

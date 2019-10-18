/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

#include <SofaQtQuickGUI/PythonConsole.h>

#include <SofaPython3/PythonEnvironment.h>
#include <pybind11/embed.h>
namespace py = pybind11;

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
        py::scoped_interpreter guard{};

        py::module c = py::module::import(PythonConsoleModule::Name().c_str());

        QFile pythonFile(":/python/PythonConsole.py");
        pythonFile.open(QIODevice::ReadOnly | QIODevice::Text);
        py::exec(QTextStream(&pythonFile).readAll().toStdString());

        redirect = true;
    }
}

}

}

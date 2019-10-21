#include "SofaQtQuick_PythonEnvironment.h"
#include <QString>
#include <sofa/helper/logging/Messaging.h>

namespace sofaqtquick {

namespace py = pybind11;
using namespace pybind11::literals;

py::dict PythonEnvironment::m_globals;

void PythonEnvironment::Init()
{
    static bool inited {false};
    if(inited)
        return;
    /// Loads the buildints then Sofa.
    m_globals = py::module::import("__main__").attr("__dict__");
    m_globals["Sofa"] = py::module::import("Sofa");
    m_globals["Sofa.Core"] = py::module::import("Sofa.Core");
    m_globals["SofaQtQuick"] = py::module::import("SofaQtQuick");
    inited=true;
}

QString PythonEnvironment::GetPythonModuleDocstring(const QString& modulePath)
{
    try {
        py::dict locals = py::dict ("modulePath"_a=modulePath.toStdString());
        py::str res = py::eval("SofaQtQuick.getPythonModuleDocstring(modulePath)", m_globals, locals);
        return QString::fromStdString(py::cast<std::string>(res));
    } catch (std::exception& e)
    {
        msg_error("PythonEnvironment") << "Unable to getPythonModuleDocstring for: " << modulePath.toStdString()
                                       << msgendl
                                       << e.what();
    }
    return QString("");
}

py::dict PythonEnvironment::GetPythonModuleContent(const QString& moduleDir, const QString& moduleName)
{
    try {
        py::dict locals = py::dict ("moduleDir"_a=moduleDir.toStdString(),
                                    "moduleName"_a=moduleName.toStdString());
        py::dict res = py::eval("SofaQtQuick.getPythonModuleContent(moduleDir, moduleName)", m_globals, locals);
        return res;
    } catch (std::exception& e)
    {
        msg_error("PythonEnvironment") << "Unable to getPythonModuleContent"
                                       << msgendl
                                       << e.what();
    }
    return py::none();
}

bool PythonEnvironment::IsASofaPythonModule(const QString &modulePath)
{
    QString docstring = GetPythonModuleDocstring(modulePath);
    return docstring.contains("type: SofaContent");
}

}/// namespace sofaqtquick

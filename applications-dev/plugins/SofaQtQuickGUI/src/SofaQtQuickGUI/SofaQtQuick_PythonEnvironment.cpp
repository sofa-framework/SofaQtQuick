
#include "SofaQtQuick_PythonEnvironment.h"
#include <SofaPython3/PythonEnvironment.h>
#include <QString>
#include <sofa/helper/logging/Messaging.h>
#include <sofa/core/objectmodel/Base.h>
namespace sofaqtquick {

namespace py = pybind11;
using namespace pybind11::literals;

py::dict PythonEnvironment::m_globals;

void PythonEnvironment::Init()
{
    sofapython3::PythonEnvironment::gil acquire;

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

py::object PythonEnvironment::CallFunction(const QString& modulePath, const QString& functionName,
                                           py::list args, py::dict kwargs, sofa::core::objectmodel::Base* ctx)
{
    sofapython3::PythonEnvironment::gil acquire;

    try
    {
        py::dict locals = py::dict ("modulePath"_a=modulePath.toStdString(),
                                    "functionName"_a=functionName.toStdString(),
                                    "args"_a=args,
                                    "kwargs"_a=kwargs);
        return py::eval("SofaQtQuick.callFunction(modulePath, functionName, *args, **kwargs)", m_globals, locals);
    } catch (std::exception& e)
    {
        if(ctx)
            msg_error(ctx) << "Unable to call python code: " << functionName.toStdString()
                                           << msgendl
                                           << e.what();
        else
            msg_error("PythonEnvironment") << "Unable to call python code: " << functionName.toStdString()
                                       << msgendl
                                       << e.what();

    }
    return py::none();
}


QString PythonEnvironment::GetPythonModuleDocstring(const QString& modulePath)
{
    sofapython3::PythonEnvironment::gil acquire;

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
    sofapython3::PythonEnvironment::gil acquire;

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
    return py::dict();
}

bool PythonEnvironment::IsASofaPythonModule(const QString &modulePath)
{
    sofapython3::PythonEnvironment::gil acquire;

    QString docstring = GetPythonModuleDocstring(modulePath);
    return docstring.contains("type: SofaContent");
}

}/// namespace sofaqtquick

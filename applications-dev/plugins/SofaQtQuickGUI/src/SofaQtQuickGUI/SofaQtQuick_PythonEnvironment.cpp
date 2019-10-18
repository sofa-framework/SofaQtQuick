#include "SofaQtQuick_PythonEnvironment.h"
#include <QString>
#include <sofa/helper/logging/Messaging.h>

namespace sofaqtquick {

namespace py = pybind11;

py::dict PythonEnvironment::getPythonScriptContent(const QString& moduleDir, const QString& moduleName)
{
    py::module m = py::module::import("SofaQtQuick");
    m.reload();

    py::dict dict = m.attr("getPythonScriptContent")(moduleDir.toStdString(), moduleName.toStdString());
    return dict;
}

py::dict PythonEnvironment::m_globals;

void PythonEnvironment::Init()
{
    static bool inited {false};
    if(inited)
        return;
    m_globals["Sofa"] = py::module::import("Sofa");
    m_globals["Sofa.Core"] = py::module::import("Sofa.Core");
    m_globals["SofaQtQuick"] = py::module::import("SofaQtQuick");
    inited=true;
}

QString PythonEnvironment::getPythonModuleDocstring(const QString& modulePath)
{
    try {
        py::dict local;
        local["modulePath"] = modulePath.toStdString();
        py::exec("SofaQtQuick.getPythonModuleDocstring(modulePath)", m_globals, local);
    } catch (std::exception& e)
    {
        msg_error("MINCE") << e.what();
    }
    return QString("");
}

}  // namespace sofaqtquick

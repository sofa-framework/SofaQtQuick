#include "SofaQtQuick_PythonEnvironment.h"

namespace sofaqtquick {

namespace py = pybind11;

py::dict PythonEnvironment::getPythonScriptContent(const std::string& moduleDir, const std::string& moduleName)
{
    py::module m = py::module::import("SofaQtQuick");

    py::dict dict = m.attr("getPythonScriptContent")(moduleDir, moduleName);
    return dict;
}


std::string PythonEnvironment::getPythonScriptDocstring(const std::string& moduleDir, const std::string& moduleName)
{
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("insert")(1, moduleDir.c_str());
    py::module m = py::module::import(moduleName.c_str());
    return py::str(m.doc());
}

}  // namespace sofaqtquick

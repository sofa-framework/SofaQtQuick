#include "SofaQtQuick_PythonEnvironment.h"

namespace sofaqtquick {

namespace py = pybind11;

py::dict PythonEnvironment::getPythonScriptContent(const std::string& moduleDir, const std::string& moduleName)
{
    py::module m = py::module::import("SofaQtQuick");
    m.reload();

    py::dict dict = m.attr("getPythonScriptContent")(moduleDir, moduleName);
    return dict;
}


bool PythonEnvironment::getPythonScriptDocstring(const std::string& moduleDir, const std::string& moduleName, std::string& docstring)
{
    try {
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(1, moduleDir.c_str());
        py::module m = py::module::import(moduleName.c_str());
        m.reload();
        docstring = py::str(m.doc());
    } catch (std::exception& e) {
        docstring = e.what();
        return false;
    }
    return true;
}

}  // namespace sofaqtquick

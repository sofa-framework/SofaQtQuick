#include "SofaQtQuick_PythonEnvironment.h"


#include <pybind11/embed.h>
#include <pybind11/eval.h>
namespace py = pybind11;

namespace sofaqtquick {

std::map<std::string, std::pair<std::string, std::string>> PythonEnvironment::getPythonScriptContent(const std::string& moduleDir, const std::string& moduleName)
{
    py::module m = py::module::import("SofaQtQuick");

    py::dict dict = m.attr("getPythonScriptContent")(moduleDir, moduleName);
    std::map<std::string, std::pair<std::string, std::string>> map;

    for (auto item : dict)
    {
        std::pair<std::string, std::string> stringmap;
        if (item.second.is(py::none()))
        {
            map[py::str(item.first)] = stringmap;
            continue;
        }
        else
        {
            for (auto func : py::cast<py::dict>(item.second))
            {
                stringmap.first = py::str(func.first);
                stringmap.second = py::str(func.second);
            }
            map[py::str(item.first)] = stringmap;
        }
    }
    return map;
}


std::string PythonEnvironment::getPythonScriptDocstring(const std::string& moduleDir, const std::string& moduleName)
{
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("insert")(1, moduleDir.c_str());
    py::module m = py::module::import(moduleName.c_str());
    return py::str(m.doc());
}

}  // namespace sofaqtquick

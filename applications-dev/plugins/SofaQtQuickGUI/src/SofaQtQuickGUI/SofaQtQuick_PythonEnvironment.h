#pragma once

#include "config.h"

#include <string>
#include <map>

#pragma push_macro("slots")
#undef slots
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#pragma pop_macro("slots")

namespace sofaqtquick {

class SOFA_SOFAQTQUICKGUI_API PythonEnvironment
{
public:
    /// Retrieves the global docstring present in a python script
    static bool getPythonScriptDocstring(const std::string& moduleDir, const std::string& moduleName, std::string& docstring);

    /// Retrieves a map of all callable functions present in a python script.
    /// The map contains as keys the name of the callable functions,
    /// and as values their type (functoin, class, prefab, controller...) and docstring
    static pybind11::dict getPythonScriptContent(const std::string& moduleDir, const std::string& moduleName);

};

}  // namespace sofaqtquick

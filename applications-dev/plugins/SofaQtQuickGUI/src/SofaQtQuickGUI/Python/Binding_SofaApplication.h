#pragma once

#include <pybind11/pybind11.h>

namespace sofapython3
{
/// Makes an alias for the pybind11 namespace to increase readability.
namespace py { using namespace pybind11; }

void moduleAddSofaApplication(py::module& m);
 
}  // namespace sofapython3

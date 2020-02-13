/********************************************************************
 Contributors:
    - bruno.josue.marques@inria.fr
********************************************************************/

#include <pybind11/eval.h>
namespace py = pybind11;

#include <SofaPython3/Sofa/Core/Binding_Base.h>
#include <SofaPython3/Sofa/Core/Binding_BaseObject.h>
#include <SofaPython3/Sofa/Core/Binding_BaseData.h>
#include <SofaPython3/Sofa/Core/Data/Binding_DataContainer.h>
#include "Binding_SofaApplication.h"

#include <SofaQtQuickGUI/config.h>

namespace sofapython3
{

PYBIND11_MODULE(SofaApplication, m) {
    m.doc() = R"doc(
              Binding for the SofaQtQuick GUI
              -----------------------------------

              Provides python bindings to control the SofaQtQuick user interface

              Example of use:

              .. code-block:: python

                import SofaApplication
                SofaApplication.setProjectDiretory(\"../\")

              .. autosummary::
                  :toctree: _autosummary/_autosummary


              )doc";

    py::module::import("Sofa.Core");

    moduleAddSofaApplication(m);
}

}  // namespace sofapython3


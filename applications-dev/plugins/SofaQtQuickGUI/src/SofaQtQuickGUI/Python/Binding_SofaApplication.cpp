
#include <SofaPython3/DataHelper.h>
#include <SofaPython3/PythonFactory.h>
#include <SofaPython3/Sofa/Core/Binding_Base.h>
#include <SofaPython3/Sofa/Core/Binding_BaseData.h>

#include <SofaQtQuickGUI/SofaBaseApplication.h>

#include "Binding_SofaApplication.h"
#include "Binding_SofaApplication_doc.h"

namespace sofapython3
{

void moduleAddSofaApplication(py::module& m)
{
    m.def("setProjectDirectory", [](const std::string& path){
        sofaqtquick::SofaBaseApplication::Instance()->setProjectDirectory(path);
    });
}

}  // namespace sofapython3

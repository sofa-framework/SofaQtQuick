
#include <SofaPython3/DataHelper.h>
#include <SofaPython3/PythonFactory.h>
#include <SofaPython3/Sofa/Core/Binding_Base.h>
#include <SofaPython3/Sofa/Core/Binding_BaseData.h>

#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <SofaQtQuickGUI/SofaProject.h>

#include "Binding_SofaApplication.h"
#include "Binding_SofaApplication_doc.h"

namespace sofapython3
{

void moduleAddSofaApplication(py::module& m)
{
    m.def("setProjectDirectory", [](const std::string& path){
        sofaqtquick::SofaBaseApplication::Instance()->setProjectDirectory(path);
    });
    m.def("getProjectDirectory", [](){
        return sofaqtquick::SofaBaseApplication::Instance()->getProjectDirectory();
    });

    m.def("getScene", [](){
        return sofaqtquick::SofaBaseApplication::Instance()->getCurrentProject()->getScene()->source().path().toStdString();
    });

    m.def("setScene", [](const std::string& path) {
        return sofaqtquick::SofaBaseApplication::Instance()->getCurrentProject()->getScene()->setSource(QUrl(QString::fromStdString(path)));
    });
    m.def("setSelectedComponent", [](Base::SPtr& component) {
        return sofaqtquick::SofaBaseApplication::Instance()->setSelectedComponent(new SofaBase(component));
    });
}

}  // namespace sofapython3

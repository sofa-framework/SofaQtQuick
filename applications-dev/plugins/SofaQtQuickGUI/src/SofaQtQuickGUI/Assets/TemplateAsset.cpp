#include "TemplateAsset.h"

#include <SofaQtQuickGUI/SofaQtQuick_PythonEnvironment.h>
#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <SofaPython3/PythonEnvironment.h>
using py3 = sofapython3::PythonEnvironment;
#include <SofaPython3/PythonFactory.h>
#include <sofa/helper/Utils.h>
#include <QFileInfo>
#include <QDir>

namespace sofaqtquick
{

TemplateAsset::TemplateAsset(std::string path, std::string extension)
    : Asset(path, extension)
{
    std::string modulepath = sofa::helper::Utils::getExecutableDirectory() + "/config/templates/assets/";
    std::string modulename =  extension + "Asset.py";
    m_templatePath = modulepath + modulename;

    QFileInfo finfo(QString::fromStdString(m_templatePath));
    if (!finfo.exists()) {
        m_assetTemplate = py::object();
        return;
    }
    try {
        m_assetTemplate = sofapython3::PythonEnvironment::importFromFile(modulename, m_templatePath);
    } catch ( pybind11::error_already_set& e) {
        msg_error("plop") << e.what();
    } catch ( std::exception& e) {
        msg_error("TemplateAsset") << e.what();
    }
    if (!m_assetTemplate.ptr())
        msg_error("TemplateAsset") << "it's all fucked!";
}

sofaqtquick::bindings::SofaNode* TemplateAsset::create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName)
{
    py3::gil lock;
    if(!m_assetTemplate.ptr()) {
        msg_error("TemplateAsset") << "no function \"create\" in template script " << m_templatePath;
        return nullptr;
    }

    std::string name = assetName.toStdString();
    if (assetName == "") {
        QFileInfo finfo(m_path.c_str());
        name = finfo.baseName().toStdString();
    }
    sofa::simulation::graph::DAGNode::SPtr assetNode = sofa::core::objectmodel::New<sofa::simulation::graph::DAGNode>(name);
    try {
        py::object foo = m_assetTemplate.attr("create");
        foo( sofapython3::PythonFactory::toPython(assetNode.get()), py::str(name), py::str(m_path));

        auto params = sofa::core::ExecParams::defaultInstance();
        assetNode->init(params);
        if (!parent->addChild(assetNode.get())) return nullptr;
        return new sofaqtquick::bindings::SofaNode(assetNode, dynamic_cast<QObject*>(this));

    } catch (std::exception& e) {
        msg_warning(parent->self()) << "Couldn't create asset " << m_path << " from template script "
                                    << m_templatePath << "\n" << e.what();
        return nullptr;
    }
}

QUrl TemplateAsset::getIconPath()
{
    py3::gil lock;
    if(!m_assetTemplate.ptr())
        return QUrl("qrc:/icon/ICON_FILE_BLANK");
    try {
        return QUrl(QString::fromStdString(py::cast<std::string>(m_assetTemplate.attr("icon_path"))));
    } catch (std::exception& e) {
        msg_warning("TemplateAsset") << "could not infer icon from template file " << m_templatePath << ".\n" << e.what();
        return QUrl("qrc:/icon/ICON_FILE_BLANK");
    }
}

QString TemplateAsset::getTypeString()
{
    py3::gil lock;
    try {
        if(!m_assetTemplate.ptr()) {
            return "unknown file type";
        }
        return QString::fromStdString(py::cast<std::string>(m_assetTemplate.attr("type_string")));
    } catch (std::exception& e) {
        msg_warning("TemplateAsset") << "could not infer typestring from template file " << m_templatePath << ".\n" << e.what();
        return "unknown file type";
    }
}

bool TemplateAsset::getIsSofaContent()
{
    if(!m_assetTemplate.ptr())
        return false;
    return true;
}

QUrl TemplateAsset::getAssetInspectorWidget() {
    py3::gil lock;
    if(!m_assetTemplate.ptr())
        return QUrl("qrc:/SofaBasics/DynamicContent_Error.qml");
    QFileInfo finfo(m_templatePath.c_str());
    QFileInfo qmlfinfo(finfo.dir().path() + "/" + finfo.baseName() + ".qml");
    std::cout << qmlfinfo.filePath().toStdString() << std::endl;
    if (qmlfinfo.exists()) {
        return QUrl(qmlfinfo.filePath());
    }
    try {
        return QUrl(QString::fromStdString(py::cast<std::string>(m_assetTemplate.attr("inspector_widget_path"))));
    } catch (std::exception& e) {
        msg_warning("TemplateAsset") << "could not infer asset widget from template file " << m_templatePath << ".\n" << e.what();
        return QUrl("qrc:/SofaBasics/DynamicContent_Error.qml");
    }
}

QVariant TemplateAsset::callAssetScriptFunction(const QString& funcName)
{
    py3::gil lock;
    if(!m_assetTemplate.ptr())
        return QVariant();
    else {
        try {
            return QVariant::fromValue(QString::fromStdString(
                        py::cast<std::string>(
                            m_assetTemplate.attr(
                                py::str(funcName.toStdString())
                                )(py::str(m_path))
                            )
                        ));
        } catch (std::exception& e) {
            msg_error("TemplateAsset") << e.what();
        }
    }
    return QVariant();
}

void TemplateAsset::openThirdParty() {
    py3::gil lock;
    if(!m_assetTemplate.ptr())
        SofaBaseApplication::Instance()->openInEditor(path());
    else {
        try {
            m_assetTemplate.attr("openThirdParty")(py::str(m_path));
        } catch (std::exception& e) {
            msg_error("TemplateAsset") << e.what();
            SofaBaseApplication::Instance()->openInEditor(path());
        }
    }
}


} // namespace sofaqtquick

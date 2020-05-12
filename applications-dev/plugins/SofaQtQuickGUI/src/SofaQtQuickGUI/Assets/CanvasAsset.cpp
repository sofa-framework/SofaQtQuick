#include "CanvasAsset.h"

#include "AssetFactory.h"
#include "AssetFactory.inl"
#include <QmlUI/Canvas.h>
#include <QFileInfo>

namespace sofaqtquick
{


//static bool __qml = sofaqtquick::AssetFactory::registerAsset("qml", new sofaqtquick::AssetCreator<CanvasAsset>());

const CanvasAsset::LoaderMap CanvasAsset::_loaders = CanvasAsset::createLoaders();

std::map<std::string, sofaqtquick::BaseAssetLoader*> CanvasAsset::createLoaders()
{
    CanvasAsset::LoaderMap m;
    m["qml"] = new CanvasAssetLoader<BaseObject>();
    return m;
}

CanvasAsset::CanvasAsset(std::string path, std::string extension)
    : Asset(path, extension)
{
}

sofaqtquick::bindings::SofaNode* CanvasAsset::create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName)
{
    SOFA_UNUSED(assetName);
    if (_loaders.find(m_extension) == _loaders.end())
    {
        msg_error("Unknown file format.");
        return new sofaqtquick::bindings::SofaNode(nullptr);
    }
    sofa::simulation::Node::SPtr root = sofa::core::objectmodel::New<sofa::simulation::graph::DAGNode>();
    root->setName("NEWNODE");
    qmlui::Canvas::SPtr canvas = sofa::core::objectmodel::New<qmlui::Canvas>();
    canvas->d_qmlFile.setValue(m_path);
    QFileInfo f(QString::fromStdString(m_path));
    canvas->setName(f.baseName().toStdString());

    root->addObject(canvas);
    root->init(sofa::core::ExecParams::defaultInstance());
    sofa::simulation::graph::DAGNode::SPtr node = sofa::simulation::graph::DAGNode::SPtr(
                dynamic_cast<sofa::simulation::graph::DAGNode*>(root.get()));
    parent->self()->addChild(node);
    return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
}

void CanvasAsset::getDetails()
{
}

}  // namespace sofaqtquick

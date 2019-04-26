#include "PythonAsset.h"

#include <SofaPython/SceneLoaderPY.h>

#include <memory>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace sofa::qtquick
{

const QUrl PythonAsset::iconPath = QUrl("qrc:/icon/ICON_PYTHON.png");
const QString PythonAsset::typeString = "Python prefab";
const PythonAsset::LoaderMap PythonAsset::loaders = PythonAsset::createLoaders();

std::map<std::string, BaseAssetLoader*> PythonAsset::createLoaders()
{
    PythonAsset::LoaderMap m;
    m["pyscn"] = new PythonAssetLoader<BaseObject>();
    m["prefab"] = new PythonAssetLoader<BaseObject>();
    m["pyctl"] = new PythonAssetLoader<BaseObject>();
    m["pyeng"] = new PythonAssetLoader<BaseObject>();
    m["py"] = new PythonAssetLoader<BaseObject>();
    return m;
}

PythonAsset::PythonAsset(std::string path, std::string extension)
    : Asset(path, extension)
{
}

SofaComponent* PythonAsset::getAsset()
{
    if (loaders.find(m_extension) == loaders.end() ||
            loaders.find(m_extension)->second == nullptr)
    {
        std::cout << "Unknown file format." << std::endl;
        return new SofaComponent(nullptr, this);
    }

    fs::path obj(m_path);

    std::string stem = obj.stem();
    std::string path = obj.parent_path().string();

    sofa::simulation::SceneLoaderPY scnLoader;
    Node::SPtr root;
    std::vector<std::string> args;
    args.push_back(path);
    args.push_back(stem);
    scnLoader.loadSceneWithArguments("config/templates/PythonAsset.py",
                                     args, &root);

    root->setName("NEWNODE");
    // We gotta store it somewhere...
    m_node = DAGNode::SPtr(dynamic_cast<DAGNode*>(root.get()));
    return new SofaComponent(nullptr, root.get());
}

void PythonAsset::getAssetMetaInfo()
{

}


} // namespace sofa::qtquick

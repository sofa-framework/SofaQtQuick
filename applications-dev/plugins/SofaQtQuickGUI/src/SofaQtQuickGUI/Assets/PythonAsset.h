#pragma once

#include "Asset.h"

#include <SofaPython/Binding.h>
#include <SofaPython/PythonEnvironment.h>
#include <SofaPython/PythonFactory.h>
using sofa::simulation::PythonEnvironment;


#include <QQmlListProperty>

namespace sofa::qtquick
{

template <class T>
struct PythonAssetLoader : public TBaseAssetLoader<T>{};

class PythonAssetModel : public QObject
{
    Q_OBJECT

public:
    PythonAssetModel() {}
    PythonAssetModel(std::string name, std::string type, std::string docstring = "");

    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
    Q_PROPERTY(QString type READ getType NOTIFY typeChanged)
    Q_PROPERTY(QString docstring READ getDocstring NOTIFY docstringChanged)

    Q_INVOKABLE QString getName() const;
    Q_INVOKABLE QString getType() const;
    Q_INVOKABLE QString getDocstring() const;
private:
    Q_SIGNAL void nameChanged(QString);
    Q_SIGNAL void typeChanged(QString);
    Q_SIGNAL void docstringChanged(QString);
    const std::string m_name;
    const std::string m_type;
    const std::string m_docstring;
};

class PythonAsset : public Asset
{
    Q_OBJECT
public:
    PythonAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* getAsset(const std::string& assetName = "") override;
    virtual void getDetails() override;

    Q_PROPERTY(QQmlListProperty<sofa::qtquick::PythonAssetModel> scriptContent READ getScriptContent)

    QQmlListProperty<sofa::qtquick::PythonAssetModel> getScriptContent();
protected:
    Q_INVOKABLE virtual QString getTypeString() override { return "Python prefab"; }
    Q_INVOKABLE virtual QUrl getIconPath() override { return QUrl("qrc:/icon/ICON_PYTHON.png"); }
    Q_INVOKABLE virtual bool getIsSofaContent() override
    {
        if (m_extension == "py")
        {
            QString docstring(PythonEnvironment::getPythonModuleDocstring(m_path).c_str());
            if (docstring.contains("type: SofaContent"))
                return true;
        }
        return false;
    }

    static const LoaderMap _loaders;

    QList<sofa::qtquick::PythonAssetModel*> m_scriptContent;

public:
    static LoaderMap createLoaders();

private:
    static int content_count(QQmlListProperty<sofa::qtquick::PythonAssetModel>*);
    static sofa::qtquick::PythonAssetModel* get_content(QQmlListProperty<sofa::qtquick::PythonAssetModel>*, int);
    static void clear_content(QQmlListProperty<sofa::qtquick::PythonAssetModel>*);

    int content_count();
    sofa::qtquick::PythonAssetModel* get_content(int);
    void clear_content();
};

} // namespace sofa::qtquick


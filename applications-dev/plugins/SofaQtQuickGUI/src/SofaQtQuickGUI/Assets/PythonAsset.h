#pragma once

#include "Asset.h"

#include <SofaPython/Binding.h>
#include <SofaPython/PythonEnvironment.h>
#include <SofaPython/PythonFactory.h>
using sofa::simulation::PythonEnvironment;

namespace sofa::qtquick
{

template <class T>
struct PythonAssetLoader : public TBaseAssetLoader<T>{};

class PythonAssetModel : public QObject
{
    Q_OBJECT

public:
    PythonAssetModel(std::string name, std::string type, std::string docstring = "");

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString docstring READ docstring NOTIFY docstringChanged)

private:
    Q_SIGNAL void nameChanged(QString);
    QString name() const;
    const std::string m_name;

    Q_SIGNAL void typeChanged(QString);
    QString type() const;
    const std::string m_type;

    Q_SIGNAL void docstringChanged(QString);
    QString docstring() const;
    const std::string m_docstring;
};

class PythonAsset : public Asset
{
    Q_OBJECT
public:
    PythonAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* getAsset(const std::string& assetName = "") override;
    virtual void getDetails() override;

    Q_PROPERTY(QList<QObject*> scriptContent READ scriptContent NOTIFY scriptContentChanged)

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

    Q_INVOKABLE QList<QObject*> scriptContent() { getDetails(); return m_scriptContent; }
    Q_SIGNAL void scriptContentChanged(const QList<QObject*>& content);

    static const LoaderMap _loaders;

    QList<QObject*> m_scriptContent;

public:
    static LoaderMap createLoaders();
};

} // namespace sofa::qtquick


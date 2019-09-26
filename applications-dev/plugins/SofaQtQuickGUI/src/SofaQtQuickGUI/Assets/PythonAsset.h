#pragma once

#include "Asset.h"
#include "SofaQtQuickGUI/SofaQtQuick_PythonEnvironment.h"

#include <QQmlListProperty>
#include <QProcess>

#include <experimental/filesystem>

namespace sofa::qtquick
{

template <class T>
struct PythonAssetLoader : public TBaseAssetLoader<T>{};

class PythonAssetModel : public QObject
{
    Q_OBJECT

public:
    PythonAssetModel() {}
    PythonAssetModel(std::string name, std::string type, std::string docstring, std::vector<std::string> params, std::string sourcecode);

    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString type READ getType WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString docstring READ getDocstring WRITE setDocstring NOTIFY docstringChanged)
    Q_PROPERTY(QStringList params READ getParams WRITE setParams NOTIFY paramsChanged)
    Q_PROPERTY(QString sourcecode READ getSourceCode WRITE setSourceCode NOTIFY sourceCodeChanged)

    Q_INVOKABLE const QString& getName() const;
    Q_INVOKABLE const QString& getType() const;
    Q_INVOKABLE const QString& getDocstring() const;
    Q_INVOKABLE const QStringList& getParams() const;
    Q_INVOKABLE const QString& getSourceCode() const;

    Q_INVOKABLE void setName(const QString& name);
    Q_INVOKABLE void setType(const QString& type);
    Q_INVOKABLE void setDocstring(const QString& docstring);
    Q_INVOKABLE void setParams(const QStringList& params);
    Q_INVOKABLE void setSourceCode(const QString& sourcecode);
private:
    Q_SIGNAL void nameChanged(QString);
    Q_SIGNAL void typeChanged(QString);
    Q_SIGNAL void docstringChanged(QString);
    Q_SIGNAL void paramsChanged(QStringList);
    Q_SIGNAL void sourceCodeChanged(QString);

    QString m_name;
    QString m_type;
    QString m_docstring;
    QStringList m_params;
    QString m_sourcecode;
};

class PythonAsset : public Asset
{
    Q_OBJECT
public:
    PythonAsset() : Asset("", "") {}
    PythonAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName = "") override;
    virtual void getDetails() override;
    virtual QUrl getAssetInspectorWidget() override;

    Q_PROPERTY(QVariantList scriptContent READ scriptContent NOTIFY scriptContentChanged)
    Q_PROPERTY(bool isScene READ isScene NOTIFY isSceneChanged)

protected:
    Q_INVOKABLE virtual QString getTypeString() override { return "Python prefab"; }
    Q_INVOKABLE virtual QUrl getIconPath() override { return QUrl("qrc:/icon/ICON_PYTHON.png"); }
    Q_INVOKABLE virtual bool getIsSofaContent() override
    {
        if (m_extension == "py")
        {
            namespace fs = std::experimental::filesystem;

            fs::path p(m_path);
            auto module = p.stem();
            auto path = p.parent_path();
            QString docstring(sofaqtquick::PythonEnvironment::getPythonScriptDocstring(path, module).c_str());
            if (docstring.contains("type: SofaContent"))
                return true;
        }
        if (m_extension == "pyscn" || m_extension == "py3")
        {
            namespace fs = std::experimental::filesystem;

            fs::path p(m_path);
            auto module = p.stem();
            auto path = p.parent_path();
            QProcess process;
            process.start("/bin/mkdir", QStringList() << "-p" << "/tmp/runSofa2");
            process.waitForFinished(-1);
            process.start("/bin/cp", QStringList() << p.string().c_str() << QString("/tmp/runSofa2/") + module.c_str() + ".py");
            process.waitForFinished(-1);
            path = "/tmp/runSofa2";
            QString docstring(sofaqtquick::PythonEnvironment::getPythonScriptDocstring(path, module).c_str());
            if (docstring.contains("type: SofaContent"))
                return true;
        }
        return false;
    }

    static const LoaderMap _loaders;


public:
    static LoaderMap createLoaders();

private:
    bool isScene();
    Q_SIGNAL void isSceneChanged(bool);
    QVariantList scriptContent();
    Q_SIGNAL void scriptContentChanged(QVariantList);
    QList<sofa::qtquick::PythonAssetModel*> m_scriptContent;
};

} // namespace sofa::qtquick


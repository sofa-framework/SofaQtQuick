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

class PythonParam : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString value READ getValue WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString annotation READ getAnnotation WRITE setAnnotation NOTIFY annotationChanged)

    Q_SIGNAL void nameChanged(QString);
    Q_SIGNAL void valueChanged(QString);
    Q_SIGNAL void annotationChanged(QString);

    QString m_name;
    QString m_value;
    QString m_annotation;

public:
    Q_INVOKABLE const QString& getName() const { return m_name; }
    Q_INVOKABLE const QString& getValue() const { return m_value; }
    Q_INVOKABLE const QString& getAnnotation() const { return m_annotation; }

    Q_INVOKABLE void setName(const QString& name) { m_name = name; }
    Q_INVOKABLE void setValue(const QString& value) { m_value = value; }
    Q_INVOKABLE void setAnnotation(const QString& annotation) { m_annotation = annotation; }

public:
    PythonParam(const std::string& name, const std::string& value, const std::string& annotation)
        : m_name(name.c_str()),
          m_value(value.c_str()),
          m_annotation(annotation.c_str()) {}
};


class PythonAssetModel : public QObject
{
    Q_OBJECT


public:
    PythonAssetModel() {}
    PythonAssetModel(std::string name, std::string type, std::string docstring, std::string sourcecode, QList<QObject*> params);

    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString type READ getType WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString docstring READ getDocstring WRITE setDocstring NOTIFY docstringChanged)
    Q_PROPERTY(QList<QObject*> params READ getParams WRITE setParams NOTIFY paramsChanged)
    Q_PROPERTY(QString sourcecode READ getSourceCode WRITE setSourceCode NOTIFY sourceCodeChanged)

    Q_INVOKABLE void openSettings();

    Q_INVOKABLE const QString& getName() const;
    Q_INVOKABLE const QString& getType() const;
    Q_INVOKABLE const QString& getDocstring() const;
    Q_INVOKABLE const QList<QObject*>& getParams() const;
    Q_INVOKABLE const QString& getSourceCode() const;

    Q_INVOKABLE void setName(const QString& name);
    Q_INVOKABLE void setType(const QString& type);
    Q_INVOKABLE void setDocstring(const QString& docstring);
    Q_INVOKABLE void setParams(const QList<QObject*>& params);
    Q_INVOKABLE void setSourceCode(const QString& sourcecode);
private:
    Q_SIGNAL void nameChanged(QString);
    Q_SIGNAL void typeChanged(QString);
    Q_SIGNAL void docstringChanged(QString);
    Q_SIGNAL void paramsChanged(QList<QObject*>);
    Q_SIGNAL void sourceCodeChanged(QString);

    QString m_name;
    QString m_type;
    QString m_docstring;
    QString m_sourcecode;
    QList<QObject*> m_params;
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

protected:
    Q_INVOKABLE virtual QString getTypeString() override { return "Python prefab"; }
    Q_INVOKABLE virtual QUrl getIconPath() override { return QUrl("qrc:/icon/ICON_PYTHON.png"); }
    Q_INVOKABLE virtual bool getIsSofaContent() override ;

    static const LoaderMap _loaders;


public:
    static LoaderMap createLoaders();

private:
    virtual bool isScene() override;
    QVariantList scriptContent();
    Q_SIGNAL void scriptContentChanged(QVariantList);
    QList<sofa::qtquick::PythonAssetModel*> m_scriptContent;
};

} // namespace sofa::qtquick


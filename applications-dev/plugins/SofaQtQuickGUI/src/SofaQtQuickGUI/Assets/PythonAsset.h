#pragma once

#include "Asset.h"
#include "SofaQtQuickGUI/SofaQtQuick_PythonEnvironment.h"

#include <QQmlListProperty>
#include <QFile>
#include <QProcess>
#include <QVariantMap>

#include <experimental/filesystem>

namespace sofaqtquick
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


class PythonAsset : public Asset
{
    Q_OBJECT
public:
    PythonAsset() : Asset("", "") {}
    PythonAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* create(sofaqtquick::bindings::SofaNode* parent,
                                                    const QString& assetName = "") override;
    Q_INVOKABLE virtual void getDetails() override;
    virtual QUrl getAssetInspectorWidget() override;

    Q_PROPERTY(QVariantList scriptContent READ scriptContent NOTIFY scriptContentChanged)
    Q_PROPERTY(QVariantList scriptActions READ scriptActions NOTIFY scriptActionsChanged)

protected:
    Q_INVOKABLE virtual QString getTypeString() override { return "Python prefab"; }
    Q_INVOKABLE virtual QUrl getIconPath() override { return isScene() ? QUrl("qrc:/icon/ICON_PYSCN.png") : QUrl("qrc:/icon/ICON_PYTHON.png"); }
    Q_INVOKABLE virtual bool getIsSofaContent() override ;

    static const LoaderMap _loaders;


public:
    static LoaderMap createLoaders();

private:
    QString getTemporaryFileName(const QString& inFile) const;
    void copyFileToCache(const QString& inPath, const QString& outFile) const;

    virtual bool isScene() override;
    QVariantList scriptContent();
    QVariantList scriptActions();
    Q_SIGNAL void scriptContentChanged(QVariantList);
    Q_SIGNAL void scriptActionsChanged(QVariantList);
    QMap<QString, QVariantMap> m_assetsContent;
};

} // namespace sofaqtquick


#pragma once

#include "Asset.h"

#include <SofaPython3/PythonEnvironment.h>
#include <SofaPython3/PythonFactory.h>
namespace py = pybind11;

namespace sofaqtquick
{

template <class T>
struct TemplateAssetLoader : public TBaseAssetLoader<T>{};

class TemplateAsset : public Asset
{
    Q_OBJECT
  public:
    TemplateAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName = "") override;
    virtual void getDetails() override {}
    QUrl getAssetInspectorWidget() override;

    Q_INVOKABLE QVariant callAssetScriptFunction(const QString& funcName);
protected:
    Q_INVOKABLE virtual QString getTypeString() override;
    Q_INVOKABLE virtual QUrl getIconPath() override;
    Q_INVOKABLE virtual bool getIsSofaContent() override;
    Q_INVOKABLE virtual void openThirdParty() override;
private:
    py::module m_assetTemplate;
    std::string m_templatePath;
};

} // namespace sofaqtquick

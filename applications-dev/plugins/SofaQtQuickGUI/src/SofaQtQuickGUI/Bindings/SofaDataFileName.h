#pragma once

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Bindings/SofaData.h>

#include <sofa/core/objectmodel/DataFileName.h>

namespace sofaqtquick::bindings
{

namespace _sofadatafilename_
{
using sofa::core::objectmodel::BaseData;
using sofa::core::objectmodel::DataFileName;


class SOFA_SOFAQTQUICKGUI_API SofaDataFileName : public SofaData
{
    Q_OBJECT

public:
    SofaDataFileName(BaseData* data);

    Q_INVOKABLE bool isDirectory()
    {
        return dynamic_cast<DataFileName*>(rawData())->isDirectory();
    }
    Q_INVOKABLE QString getRelativePath()
    {
        return QString::fromStdString(dynamic_cast<DataFileName*>(rawData())->getRelativePath());
    }
    Q_INVOKABLE QString getFullPath()
    {
        return QString::fromStdString(dynamic_cast<DataFileName*>(rawData())->getFullPath());
    }
    Q_INVOKABLE QString getAbsolutePath()
    {
        return QString::fromStdString(dynamic_cast<DataFileName*>(rawData())->getAbsolutePath());
    }
    Q_INVOKABLE QString getExtension()
    {
        return QString::fromStdString(dynamic_cast<DataFileName*>(rawData())->getExtension());
    }
};

}  // namespace _sofadatafilename_

using _sofadatafilename_::SofaDataFileName;

}  // namespace sofaqtquick::bindings

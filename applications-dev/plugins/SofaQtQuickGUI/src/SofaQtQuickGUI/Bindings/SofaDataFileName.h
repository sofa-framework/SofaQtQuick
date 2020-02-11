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
};

}  // namespace _sofadatafilename_

using _sofadatafilename_::SofaDataFileName;

}  // namespace sofaqtquick::bindings

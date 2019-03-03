/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/
#include "SofaData.h"
#include "../DataHelper.h"

namespace sofaqtquick::bindings::_sofadata_
{

SofaData::SofaData(BaseData* self)
{
    m_self = self;
}

QVariant SofaData::value() const
{
    return sofaqtquick::helper::createQVariantFromData(m_self);
}

bool SofaData::setValue(const QVariant& value)
{
    if(sofaqtquick::helper::setDataValueFromQVariant(m_self, value))
    {
        emit valueChanged(value);
        return true;
    }
    return false;
}


}

#include <GL/glew.h>
#include <SofaQtQuickGUI/Bindings/SofaData.h>
#include <SofaQtQuickGUI/Bindings/SofaComponent.h>
#include <SofaQtQuickGUI/SofaScene.h>

#include <QString>

namespace sofa
{

namespace qtquick
{

using namespace sofa::core::objectmodel;

SofaData::SofaData(const SofaComponent* sofaComponent, const sofa::core::objectmodel::BaseData* data) : QObject(),
    mySofaComponent(new SofaComponent(*sofaComponent)),
    myData(data)
{
    mySofaComponent->setParent(this);
}

SofaData::SofaData(SofaScene* sofaScene, const sofa::core::objectmodel::Base* base, const sofa::core::objectmodel::BaseData* data) : QObject(),
    mySofaComponent(new SofaComponent(sofaScene, base)),
    myData(data)
{
    mySofaComponent->setParent(this);
}

SofaData::SofaData(const SofaData& sofaData) : QObject(),
    mySofaComponent(new SofaComponent(*sofaData.mySofaComponent)),
    myData(sofaData.myData)
{
    mySofaComponent->setParent(this);
}

SofaComponent* SofaData::sofaComponent() const
{
    return mySofaComponent;
}

QVariantMap SofaData::object()
{
    if(mySofaComponent)
    {
        SofaScene* sofaScene = mySofaComponent->sofaScene();
        if(sofaScene)
        {
            const BaseData* data = SofaData::data();
            if(data)
                return sofaScene->dataObject(data);
        }
    }

    return QVariantMap();
}

QVariant SofaData::value() const
{
    const BaseData* data = SofaData::data();
    if(data)
        return SofaScene::dataValue(data);

    return QVariant();
}

bool SofaData::setValue(const QVariant& value)
{
    BaseData* data = SofaData::data();
    if(data)
        return SofaScene::setDataValue(data, value);

    return false;
}

bool SofaData::setLink(const QString& path)
{
    BaseData* data = SofaData::data();
    if(data)
    {
        std::streambuf* backup(std::cerr.rdbuf());

        std::ostringstream stream;
        std::cerr.rdbuf(stream.rdbuf());
        bool status = SofaScene::setDataLink(data, path);
        std::cerr.rdbuf(backup);

        return status;
    }

    return false;
}

BaseData* SofaData::data()
{
    return const_cast<BaseData*>(static_cast<const SofaData*>(this)->data());
}

const BaseData* SofaData::data() const
{
    const BaseData* data = nullptr;

    // check if the base still exists hence if the data is still valid
    if(mySofaComponent)
    {
        const Base* base = mySofaComponent->base();
        if(base)
            data = myData;
    }

    return data;
}

}

}

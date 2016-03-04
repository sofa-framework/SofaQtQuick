/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

#include <GL/glew.h>
#include "SofaData.h"
#include "SofaComponent.h"
#include "SofaScene.h"

#include <QString>

namespace sofa
{

namespace qtquick
{

using namespace sofa::core::objectmodel;

SofaData::SofaData(const SofaComponent* sofaComponent, const sofa::core::objectmodel::BaseData* data) : QObject(),
    mySofaComponent(sofaComponent),
    myData(data)
{

}

SofaData::SofaData(const SofaScene* sofaScene, const sofa::core::objectmodel::Base* base, const sofa::core::objectmodel::BaseData* data) : QObject(),
    mySofaComponent(new SofaComponent(sofaScene, base)),
    myData(data)
{

}

SofaData::SofaData(const SofaData& sofaData) : QObject(),
    mySofaComponent(sofaData.mySofaComponent),
    myData(sofaData.myData)
{

}

const SofaComponent* SofaData::sofaComponent() const
{
    return mySofaComponent;
}

QVariantMap SofaData::object() const
{
    if(mySofaComponent)
    {
        const SofaScene* sofaScene = mySofaComponent->sofaScene();
        if(sofaScene)
        {
            const BaseData* data = SofaData::data();
            if(data)
                return sofaScene->dataObject(data);
        }
    }

    return QVariantMap();
}

QVariant SofaData::value()
{
    BaseData* data = SofaData::data();
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

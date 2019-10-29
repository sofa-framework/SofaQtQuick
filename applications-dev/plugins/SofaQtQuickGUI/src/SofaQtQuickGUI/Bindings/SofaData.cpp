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

#include <sofa/core/objectmodel/Base.h>
using sofa::core::objectmodel::Base;
#include <sofa/core/objectmodel/BaseObject.h>
using sofa::core::objectmodel::BaseObject;
#include <sofa/core/objectmodel/BaseNode.h>
using sofa::core::objectmodel::BaseNode;

#include <sofa/defaulttype/DataTypeInfo.h>
using sofa::defaulttype::AbstractTypeInfo;

#include "SofaData.h"
#include "../DataHelper.h"
using sofaqtquick::helper::convertDataInfoToProperties;

namespace sofaqtquick::bindings::_sofadata_
{

SofaData::SofaData(BaseData* self)
{
    m_self = self;
}

QVariant SofaData::getValue() const
{
    return sofaqtquick::helper::createQVariantFromData(m_self);
}

bool SofaData::setValue(const QVariant& value)
{
    if(sofaqtquick::helper::setDataValueFromQVariant(m_self, value))
    {
        m_self->setPersistent(true);
        return true;
    }
    return false;
}

QVariantMap SofaData::object()
{
    const BaseData* data = rawData();
    if(data)
    {
        QVariantMap map = sofaqtquick::helper::getSofaDataProperties(data);
        map.insert("sofaData", QVariant::fromValue(this));
        return map;
    }

    return QVariantMap();
}

QString SofaData::getValueType() const
{
    return QString::fromStdString(rawData()->getValueTypeString());
}

QString SofaData::getName() const
{
    return QString::fromStdString(rawData()->getName());
}

QString SofaData::getPathName() const
{
    const BaseData* data = rawData();
    const Base* owner = data->getOwner();

    QString prefix = "";
    if(owner->toBaseNode())
        prefix = QString::fromStdString(owner->toBaseNode()->getPathName());
    else if(owner->toBaseObject())
        prefix = QString::fromStdString(owner->toBaseObject()->getPathName());

    return prefix + "." + QString::fromStdString(data->getName());
}

QVariantMap SofaData::getProperties()const
{
    QVariantMap properties;
    convertDataInfoToProperties(rawData(), properties);
    return properties;
}

QString SofaData::getLinkPath() const
{
    return QString::fromStdString(rawData()->getLinkPath());
}


bool SofaData::setLink(const QString& path)
{
    BaseData* data = rawData();
    if(data)
    {
        std::streambuf* backup(std::cerr.rdbuf());

        std::ostringstream stream;
        std::cerr.rdbuf(stream.rdbuf());
        bool status = sofaqtquick::helper::setDataLink(data, path);
        std::cerr.rdbuf(backup);

        emit linkPathChanged(QString::fromStdString(data->getLinkPath()));
        return status;
    }

    return false;
}


QString SofaData::getHelp() const
{
    return QString::fromStdString(rawData()->getHelp());

}

bool SofaData::isSet() const
{
    return rawData()->isSet();
}

bool SofaData::isReadOnly() const
{
    return rawData()->isReadOnly();
}

QString SofaData::getGroup() const
{
    return QString::fromStdString(rawData()->getGroup());
}

} /// namespace sofaqtquick::bindings

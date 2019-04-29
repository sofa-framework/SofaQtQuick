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

        return status;
    }

    return false;
}

} /// namespace sofaqtquick::bindings

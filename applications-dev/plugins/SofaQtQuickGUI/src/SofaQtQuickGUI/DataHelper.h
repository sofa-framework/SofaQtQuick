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
#pragma once

#include <QVariant>

/// Forward declartations.
namespace sofa::core::objectmodel
{
class BaseData;
class BaseNode;
}

/// Declaration
namespace sofaqtquick::helper
{
    using sofa::core::objectmodel::BaseData;
    using sofa::core::objectmodel::BaseNode;

    QVariant createQVariantFromData(const BaseData*);
    QVariantMap getSofaDataProperties(const BaseData* data);

    // TODO: WARNING : do not use data->read anymore but directly the correct set*Type*Value(...)
    bool setDataValueFromQVariant(BaseData* data, const QVariant& value);
    bool setDataLink(BaseData* data, const QString& link);

    BaseData* findData(BaseNode* node, const QString& path);
}

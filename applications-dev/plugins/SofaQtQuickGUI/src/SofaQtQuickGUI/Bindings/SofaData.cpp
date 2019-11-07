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

void QmlDDGNode::notifyEndEdit(const sofa::core::ExecParams *params)
{
    DDGNode::notifyEndEdit(params);
    emit valueChanged(QVariant(0));
}

void QmlDDGNode::update(){}

SofaData::SofaData(BaseData* self)
{
    m_self = self;
    m_ddgnode.self=self;

    /// Connect a dedicated node as output
    m_self->addOutput(&m_ddgnode);
    connect(&m_ddgnode, &QmlDDGNode::valueChanged, this, &SofaData::valueChanged);
}

SofaData::~SofaData()
{
    m_self->delOutput(&m_ddgnode);
    disconnect(&m_ddgnode, &QmlDDGNode::valueChanged, this, &SofaData::valueChanged);
}

bool SofaData::hasParent() const
{
    return m_self->getParent() != nullptr;
}

SofaData* SofaData::getParent() const
{
    if(m_self->getParent())
        return new SofaData(m_self->getParent());
    return nullptr;
}

void SofaData::setParent(SofaData* data)
{
    m_self->setParent(data->m_self, data->getLinkPath().toStdString());
    emit parentChanged(data);
}

QVariant SofaData::getValue()
{
    //std::cout << "SofaData::getValue: " << rawData()->getName()
    //          << " counter:" << rawData()->getCounter() << std::endl;
    m_previousValue = sofaqtquick::helper::createQVariantFromData(m_self);
    return m_previousValue;
}

bool SofaData::setValue(const QVariant& value)
{
    //std::cout << "Trying to setValue: " << value.toString().toStdString() << " counter:" << rawData()->getCounter() << std::endl;
    if(value != m_previousValue)
    {
        //std::cout << "Doin to setValue: " << value.toString().toStdString() << std::endl;
        if(sofaqtquick::helper::setDataValueFromQVariant(m_self, value))
        {
            m_self->setPersistent(true);
            m_previousValue = value;
            emit valueChanged(value);
            emit propertiesChanged(getProperties());
            return true;
        }
        return false;
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
    return QString("@")+getPathName();
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

bool SofaData::isLinkValid(const QString &path)
{
    Base* owner = rawData()->getOwner();
    BaseObject* o = owner->toBaseObject();
    BaseNode* n = owner->toBaseNode();
    BaseData* parent = nullptr;
    if (o)
        parent = sofaqtquick::helper::findData(o->getContext()->toBaseNode(), path);
    else if (n)
        parent = sofaqtquick::helper::findData(n, path);
    else {
        return false;
    }

    return (parent && rawData()->validParent(parent));
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

bool SofaData::isAutoLink() const
{
    return rawData()->isAutoLink();
}


QString SofaData::getGroup() const
{
    return QString::fromStdString(rawData()->getGroup());
}
QString SofaData::getOwnerClass() const
{
    return QString::fromStdString(rawData()->getOwnerClass());
}

int SofaData::getCounter() const
{
    return rawData()->getCounter();
}



} /// namespace sofaqtquick::bindings

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

#include <sofa/core/objectmodel/DataFileName.h>
using sofa::core::objectmodel::DataFileName;

#include <sofa/defaulttype/DataTypeInfo.h>
using sofa::defaulttype::AbstractTypeInfo;

#include "SofaData.h"
#include "../DataHelper.h"
using sofaqtquick::helper::convertDataInfoToProperties;

#include <QMessageBox>
#include <QInputDialog>

#include <SofaQtQuickGUI/SofaBaseApplication.h>
using sofaqtquick::SofaBaseApplication;
#include <SofaPython3/PythonFactory.h>

namespace sofaqtquick::bindings::_sofadata_
{

void QmlDDGNode::notifyEndEdit(const sofa::core::ExecParams *params)
{
    DDGNode::notifyEndEdit(params);
    SofaBaseApplication::requestDataViewUpdate(this);
}

void QmlDDGNode::update(){}

SofaData::SofaData(BaseData* self)
{
    m_self = self;
    m_ddgnode.m_sofadata = this;
    m_ddgnode.m_basedata = self;
    m_self->addOutput(&m_ddgnode);
}

SofaData::~SofaData()
{
    /// Register a new data view
    m_self->delOutput(&m_ddgnode);
    SofaBaseApplication::removePendingDataViewUpdate(&m_ddgnode);
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
    if (data)
        m_self->setParent(data->m_self, data->getLinkPath().toStdString());
    else m_self->setParent(nullptr);
    emit parentChanged(data);
}

QVariant SofaData::getValue()
{
    m_previousValue = sofaqtquick::helper::createQVariantFromData(m_self);
    return m_previousValue;
}

bool SofaData::setValue(const QVariant& value)
{
    if(value != m_previousValue)
    {
        _disconnect();
        //std::cout << "Doin to setValue: " << value.toString().toStdString() << std::endl;
        if(sofaqtquick::helper::setDataValueFromQVariant(m_self, value))
        {
            m_self->setPersistent(true);
            m_previousValue = value;
            emit valueChanged(value);
            emit propertiesChanged(getProperties());
            _connect();
            return true;
        }
        _connect();
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

/// This method tries to link 2 datafields of incompatible types by using a python conversion macro
/// The function first tries to import a module named "srcType2dstType"
/// If the module exists, create a callback in dstData's Owner, named dstDataName_srcType2dstType,
///  that links srcData to dstData by calling the conversion macro in the python script
/// Otherwise, we let the user create the macro in the callbacksDirectory for later use.
bool SofaData::tryLinkingIncompatibleTypes(const QString& path)
{
    Base* owner = rawData()->getOwner();
    BaseNode* root = nullptr;
    if (owner->toBaseNode() != nullptr)
        root = owner->toBaseNode()->getRoot();
    if (owner->toBaseObject() != nullptr)
        root = owner->toBaseObject()->getContext()->getRootContext()->toBaseNode();
    BaseData* srcData = sofaqtquick::helper::findData(root, path);
    if (srcData)
    {
        SofaBaseApplication::Instance()->callbacksDirectory(); // just making sure the pythonPath contains the callbacks folder...
        BaseData* dstData = rawData();
        bool ret = false;
        sofapython3::PythonEnvironment::executePython([root, srcData, dstData, &ret](){
            QString srcType = srcData->getValueTypeString().c_str();
            QString dstType = dstData->getValueTypeString().c_str();
            auto modulename = srcType.replace("<", "_").replace(">", "_").toStdString() + "2" + dstType.replace("<", "_").replace(">", "_").toStdString();
            sofapython3::PythonEnvironment::runString("import sys\nprint(sys.path)");
            sofapython3::py::module m = sofapython3::py::module::import(modulename.c_str());
            sofapython3::py::module sqq = sofapython3::py::module::import("SofaQtQuick");
            std::string engineName = "to_"+dstData->getOwner()->getName() + "_" + dstData->getName();

            if (static_cast<sofa::simulation::Node*>(root)->getObject(engineName))
                ret = true;
            else if (!m.is_none())
            {
                sqq.attr("createTypeConversionEngine")(modulename.c_str(),
                                                       sofapython3::PythonFactory::toPython(root),
                                                       sofapython3::PythonFactory::toPython(srcData),
                                                       sofapython3::PythonFactory::toPython(dstData),
                                                       srcType.toStdString().c_str(),
                                                       dstType.toStdString().c_str());
            }
            ret = true;
        });
        if (ret)
            return true;
        QString srcType = srcData->getValueTypeString().c_str();
        QString dstType = dstData->getValueTypeString().c_str();
        QString title("Warning: You're trying to link incompatible types. ");
        title = title + srcType + " != " + dstType ;
        if (QMessageBox::question(nullptr, title, tr("Do you want to create a PythonDataTrackerEngine?")) == QMessageBox::StandardButton::Yes)
        {
            auto sa = SofaBaseApplication::Instance();
            auto modulename = srcType.replace("<", "_").replace(">", "_") + "2" + dstType.replace("<", "_").replace(">", "_");

            sa->createCallback(sa->callbacksDirectory() + modulename + ".py");
            sa->openInEditor(sa->callbacksDirectory() + modulename + ".py");
        }
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

bool SofaData::isPersistent() const
{
    return rawData()->isPersistent();
}


void SofaData::setPersistent(bool persistent)
{
    m_self->setPersistent(persistent);
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

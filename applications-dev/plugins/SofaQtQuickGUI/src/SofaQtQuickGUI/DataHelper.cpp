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
#include <QJSValue>

#include <sofa/core/objectmodel/BaseData.h>
#include <sofa/core/objectmodel/BaseNode.h>
using sofa::core::objectmodel::BaseNode;

#include <sofa/core/objectmodel/BaseObject.h>
using sofa::core::objectmodel::BaseObject;

#include <sofa/defaulttype/DataTypeInfo.h>
using sofa::defaulttype::AbstractTypeInfo;

#include <sofa/helper/OptionsGroup.h>
using sofa::helper::OptionsGroup ;

#include <sofa/core/objectmodel/DataFileName.h>
using sofa::core::objectmodel::DataFileName ;

#include <sofa/helper/types/RGBAColor.h>
using sofa::helper::types::RGBAColor ;

#include <sofa/helper/system/FileSystem.h>
using sofa::helper::system::FileSystem ;

#include "DataHelper.h"


/// Declaration
namespace sofaqtquick::helper
{

using sofa::defaulttype::DataTypeInfo;
using sofa::defaulttype::DataTypeName;
using sofa::core::objectmodel::Data;

QVariant createQVariantFromData(const BaseData* data)
{
    QVariant value;

    if(!data)
        return value;

    if (dynamic_cast<const DataFileName*>(data))
        return QVariant::fromValue(QString::fromStdString(dynamic_cast<const DataFileName*>(data)->getAbsolutePath()));

    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();
    const void* valueVoidPtr = data->getValueVoidPtr();

    if(!typeinfo->Container())
    {
        if(typeinfo->Text())
            value = QString::fromStdString(typeinfo->getTextValue(valueVoidPtr, 0));
        else if(typeinfo->Scalar())
            value = typeinfo->getScalarValue(valueVoidPtr, 0);
        else if(typeinfo->Integer())
        {
            if(std::string::npos != typeinfo->name().find("bool"))
                value = 0 != typeinfo->getIntegerValue(valueVoidPtr, 0) ? true : false;
            else
                value = typeinfo->getIntegerValue(valueVoidPtr, 0);
        }
        else
        {
            value = QString::fromStdString(data->getValueString());
        }
    }
    else
    {
        size_t nbCols = typeinfo->size();
        size_t nbRows = typeinfo->size(data->getValueVoidPtr()) / nbCols;

        if(typeinfo->Text())
        {
            QVariantList values;
            values.reserve(int(nbRows));

            QVariantList subValues;
            subValues.reserve(int(nbCols));

            for(size_t j = 0; j < nbRows; j++)
            {
                subValues.clear();
                for(size_t i = 0; i < nbCols; i++)
                    subValues.append(QVariant::fromValue(QString::fromStdString((typeinfo->getTextValue(valueVoidPtr, j * nbCols + i)))));

                values.append(QVariant::fromValue(subValues));
            }

            value = values;
        }
        else if(typeinfo->Scalar())
        {
            QVariantList values;
            values.reserve(int(nbRows));

            QVariantList subValues;
            subValues.reserve(int(nbCols));

            for(size_t j = 0; j < nbRows; j++)
            {
                subValues.clear();
                for(size_t i = 0; i < nbCols; i++)
                    subValues.append(QVariant::fromValue(typeinfo->getScalarValue(valueVoidPtr, j * nbCols + i)));

                values.append(QVariant::fromValue(subValues));
            }

            value = values;
        }
        else if(typeinfo->Integer())
        {
            QVariantList values;
            values.reserve(int(nbRows));

            QVariantList subValues;
            subValues.reserve(int(nbCols));

            bool isBool = false;
            if(std::string::npos != typeinfo->name().find("bool"))
                isBool = true;

            for(size_t j = 0; j < nbRows; j++)
            {
                subValues.clear();

                if(isBool)
                    for(size_t i = 0; i < nbCols; i++)
                        subValues.append(QVariant::fromValue(0 != typeinfo->getIntegerValue(valueVoidPtr, j * nbCols + i) ? true : false));
                else
                    for(size_t i = 0; i < nbCols; i++)
                        subValues.append(QVariant::fromValue(typeinfo->getIntegerValue(valueVoidPtr, j * nbCols + i)));

                values.push_back(QVariant::fromValue(subValues));
            }

            value = values;
        }
        else
        {
            value = QString::fromStdString(data->getValueString());
        }
    }

    return value;
}

bool setDataLink(BaseData* data, const QString& link)
{
    if(!data)
        return false;

    if(link.isEmpty())
        data->setParent(nullptr);
    else
        data->setParent(link.toStdString());
    return data->getParent();
}

bool setDataValueFromQVariant(BaseData* data, const QVariant& value)
{
    if(!data)
        return false;

    if(value.isNull())
        return false;

    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();

    QVariant finalValue = value;
    if(finalValue.userType() == qMetaTypeId<QJSValue>())
        finalValue = finalValue.value<QJSValue>().toVariant();

    if(QVariant::List == finalValue.type())
    {
        QSequentialIterable valueIterable = finalValue.value<QSequentialIterable>();

        int nbCols = typeinfo->size();
        int nbRows = typeinfo->size(data->getValueVoidPtr()) / nbCols;

        if(!typeinfo->Container())
        {
            dmsg_warning("SofaQtQuickGUI") << "Trying to set a list of values on a non-container data";
            return false;
        }

        if(valueIterable.size() != nbRows)
        {
            if(typeinfo->FixedSize())
            {
                dmsg_warning("SofaQtQuickGUI") << "The new data should have the same size, should be" << nbRows << ", got" << valueIterable.size();
                return false;
            }

            //typeinfo->setSize(data, valueIterable.size()); // CRASH and does not seems to be necessary since we call data->read, but maybe it will be necessary someday
        }

        if(typeinfo->Scalar())
        {
            QString dataString;
            for(int i = 0; i < valueIterable.size(); ++i)
            {
                QVariant subFinalValue = valueIterable.at(i);
                if(QVariant::List == subFinalValue.type())
                {
                    QSequentialIterable subValueIterable = subFinalValue.value<QSequentialIterable>();
                    if(subValueIterable.size() != nbCols)
                    {
                        msg_warning("SofaQtQuickGUI") << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
                        return false;
                    }

                    for(int j = 0; j < subValueIterable.size(); ++j)
                    {
                        dataString += QString::number(subValueIterable.at(j).toDouble());
                        if(subValueIterable.size() - 1 != j)
                            dataString += ' ';
                    }
                }
                else
                {
                    dataString += QString::number(subFinalValue.toDouble());
                }

                if(valueIterable.size() - 1 != i)
                    dataString += ' ';
            }

            data->read(dataString.toStdString());
        }
        else if(typeinfo->Integer())
        {
            QString dataString;
            for(int i = 0; i < valueIterable.size(); ++i)
            {
                QVariant subFinalValue = valueIterable.at(i);
                if(QVariant::List == subFinalValue.type())
                {
                    QSequentialIterable subValueIterable = subFinalValue.value<QSequentialIterable>();
                    if(subValueIterable.size() != nbCols)
                    {
                        msg_warning("SofaQtQuickGUI") << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
                        return false;
                    }

                    for(int j = 0; j < subValueIterable.size(); ++j)
                    {
                        dataString += QString::number(subValueIterable.at(j).toLongLong());
                        if(subValueIterable.size() - 1 != j)
                            dataString += ' ';
                    }
                }
                else
                {
                    dataString += QString::number(subFinalValue.toLongLong());
                }

                if(valueIterable.size() - 1 != i)
                    dataString += ' ';
            }

            data->read(dataString.toStdString());
        }
        else if(typeinfo->Text())
        {
            QString dataString;
            for(int i = 0; i < valueIterable.size(); ++i)
            {
                QVariant subFinalValue = valueIterable.at(i);
                if(QVariant::List == subFinalValue.type())
                {
                    QSequentialIterable subValueIterable = subFinalValue.value<QSequentialIterable>();
                    if(subValueIterable.size() != nbCols)
                    {
                        msg_warning("SofaQtQuickGUI") << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
                        return false;
                    }

                    for(int j = 0; j < subValueIterable.size(); ++j)
                    {
                        dataString += subValueIterable.at(j).toString();
                        if(subValueIterable.size() - 1 != j)
                            dataString += ' ';
                    }
                }
                else
                {
                    dataString += subFinalValue.toString();
                }

                if(valueIterable.size() - 1 != i)
                    dataString += ' ';
            }

            data->read(dataString.toStdString());
        }
        else
            data->read(value.toString().toStdString());
    }
    else if(QVariant::Map == finalValue.type())
    {
        dmsg_warning("SofaQtQuickGUI") << "Map type is not supported";
        return false;
    }
    else
    {
        if(typeinfo->Text())
            data->read(value.toString().toStdString());
        else if(typeinfo->Scalar())
            data->read(QString::number(value.toDouble()).toStdString());
        else if(typeinfo->Integer())
            data->read(QString::number(value.toLongLong()).toStdString());
        else
            data->read(value.toString().toStdString());
    }

    return true;
}

QVariantMap& convertDataInfoToProperties(const BaseData* data, QVariantMap& properties)
{
    properties.insert("readOnly", data->isReadOnly());
    properties.insert("required", data->isRequired());
    properties.insert("displayed", data->isDisplayed());
    properties.insert("set", data->isSet());

    /// DataFilename are use to sœœtores path to files.
    auto aDataFilename = dynamic_cast<const DataFileName*>(data) ;

    /// OptionsGroup are used to encode a finite set of alternatives.
    auto anOptionGroup =  dynamic_cast<const Data<OptionsGroup>*>(data) ;

    /// OptionsGroup are used to encode a finite set of alternatives.
    auto aRGBAColor =  dynamic_cast<const Data<RGBAColor>*>(data) ;

    if(aDataFilename)
    {
        properties.insert("type", "FileName");
        properties.insert("url", QString::fromStdString(aDataFilename->getFullPath())) ;

        if(aDataFilename->getValueString() == ""){
            properties.insert("folderurl",  "") ;
        }else{
            std::string directory = FileSystem::getParentDirectory( aDataFilename->getFullPath() ) ;
            properties.insert("folderurl",  QString::fromStdString(directory)) ;
        }
        return properties;
    }

    if(anOptionGroup)
    {
        QStringList choices;

        const OptionsGroup& group = anOptionGroup->getValue();
        for(unsigned int i=0;i<group.size();++i)
        {
            choices.append(QString::fromStdString(group[i]));
        }
        properties.insert("type", "OptionsGroup");
        properties.insert("choices", choices);
        properties.insert("autoUpdate", true);
        return properties;
    }

    if(aRGBAColor)
    {
        properties.insert("type", "RGBAColor");
        properties.insert("autoUpdate", true);
        return properties;
    }

    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();
    if(!typeinfo->Container()){
        if(typeinfo->Integer())
        {
            if(std::string::npos != typeinfo->name().find("bool"))
            {
                properties.insert("type", "boolean");
                return properties;
            }

            properties.insert("type", "number");
            properties.insert("decimals", 0);
            if(std::string::npos != typeinfo->name().find("unsigned"))
                properties.insert("min", 0);
            return properties;
        }
        if(typeinfo->Scalar())
        {
            properties.insert("type", "number");
            properties.insert("step", 0.1);
            properties.insert("precision", 6);
            return properties;
        }

        if(typeinfo->Text())
        {
            properties.insert("type", "string");
            return properties;
        }

        /// Specific type...
        properties.insert("type",data->getValueTypeString().c_str());
        return properties;
    }

    int nbCols = int(typeinfo->BaseType()->size());
    if (nbCols == 1)
        nbCols = int(typeinfo->size());
    int nbRows = int(typeinfo->size(data->getValueVoidPtr()) / nbCols);
    properties.insert("cols", nbCols);
    properties.insert("rows", nbRows);
    if(typeinfo->FixedSize())
        properties.insert("static", true);

    const AbstractTypeInfo* baseTypeinfo = typeinfo->BaseType();
    if(baseTypeinfo->FixedSize())
        properties.insert("innerStatic", true);

    QString widget = QString::fromStdString(data->getWidget());
    if(!widget.isEmpty())
    {
        properties.insert("type",widget);
        return properties;
    }

    /// Default type...
    properties.insert("type","array");
    return properties;
}


QVariantMap getSofaDataProperties(const sofa::core::objectmodel::BaseData* data)
{
    QVariantMap object;

    if(!data)
    {
        object.insert("name", "Invalid");
        object.insert("description", "");
        object.insert("type", "");
        object.insert("group", "");
        object.insert("properties", "");
        object.insert("link", "");
        object.insert("value", "");
        return object;
    }

    // TODO:
    QString type;
    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();

    QVariantMap properties;

    if(typeinfo->Text())
    {
        type = "string";
        properties.insert("autoUpdate", true);
    }
    else if(typeinfo->Scalar())
    {
        type = "number";
        properties.insert("autoUpdate", true);
        properties.insert("step", 0.1);
        properties.insert("decimals", 14);
    }
    else if(typeinfo->Integer())
    {
        if(std::string::npos != typeinfo->name().find("bool"))
        {
            type = "boolean";
            properties.insert("autoUpdate", true);
        }
        else
        {
            type = "number";
            properties.insert("decimals", 0);
            properties.insert("autoUpdate", true);
            if(std::string::npos != typeinfo->name().find("unsigned"))
                properties.insert("min", 0);
        }
    }
    else
    {
        type = QString::fromStdString(data->getValueTypeString());
    }

    if(typeinfo->Container())
    {
        type = "array";
        int nbCols = typeinfo->size();

        properties.insert("cols", nbCols);
        if(typeinfo->FixedSize())
            properties.insert("static", true);

        const AbstractTypeInfo* baseTypeinfo = typeinfo->BaseType();
        if(baseTypeinfo->FixedSize())
            properties.insert("innerStatic", true);
    }

    /// DataFilename are use to sœœtores path to files.
    const DataFileName* aDataFilename = dynamic_cast<const DataFileName*>(data) ;

    /// OptionsGroup are used to encode a finite set of alternatives.
    const Data<OptionsGroup>* anOptionGroup =  dynamic_cast<const Data<OptionsGroup>*>(data) ;

    /// OptionsGroup are used to encode a finite set of alternatives.
    const Data<RGBAColor>* aRGBAColor =  dynamic_cast<const Data<RGBAColor>*>(data) ;

    if(aDataFilename)
    {
        type = "FileName" ;
        properties.insert("url", QString::fromStdString(aDataFilename->getFullPath())) ;

        const std::string& directory = FileSystem::getParentDirectory( aDataFilename->getFullPath() ) ;
        properties.insert("folderurl",  QString::fromStdString(directory)) ;
    }
    else if(anOptionGroup)
    {
        type = "OptionsGroup";
        QStringList choices;

        const OptionsGroup& group = anOptionGroup->getValue();
        for(unsigned int i=0;i<group.size();++i)
        {
            choices.append(QString::fromStdString(group[i]));
        }
        properties.insert("choices", choices);
        properties.insert("autoUpdate", true);

    }else if(aRGBAColor)
    {
        type = "RGBAColor";
        properties.insert("autoUpdate", true);
    }

    QString widget(QString::fromStdString(data->getWidget()));
    if(!widget.isEmpty())
        type = widget;

    properties.insert("readOnly", data->isReadOnly());
    properties.insert("required", data->isRequired());
    properties.insert("displayed", data->isDisplayed());

    //object.insert("sofaData", QVariant::fromValue(sofaData));
    object.insert("name", data->getName().c_str());
    object.insert("description", QString::fromStdString(data->getHelp()));
    object.insert("type", type);
    object.insert("group", QString::fromStdString(data->getGroup()));
    object.insert("properties", properties);
    object.insert("link", QString::fromStdString(data->getLinkPath()));
    object.insert("value", createQVariantFromData(data));

    return object;
}

BaseData* findData(BaseNode* node, const QString& path)
{
    BaseData* data = nullptr;

    if(node)
    {
        node->findDataLinkDest(data, path.toStdString(), nullptr);
    }

    return data;
}

const std::string getPathName(Base *base)
{
    BaseNode* node = base->toBaseNode();
    if(node)
        return node->getPathName();

    BaseObject* object = base->toBaseObject();
    if(object)
        return object->getPathName();

    return "invalid object";
}


}

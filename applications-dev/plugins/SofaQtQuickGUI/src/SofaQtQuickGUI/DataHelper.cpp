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

#include <sofa/defaulttype/DataTypeInfo.h>
using sofa::defaulttype::AbstractTypeInfo;

#include "DataHelper.h"

/// Declaration
namespace sofaqtquick::helper
{

QVariant createQVariantFromData(const BaseData* data)
{
    QVariant value;

    if(!data)
        return value;

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

}

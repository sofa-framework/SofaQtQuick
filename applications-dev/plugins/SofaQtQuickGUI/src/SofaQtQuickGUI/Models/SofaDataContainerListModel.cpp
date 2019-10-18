#include "SofaDataContainerListModel.h"

#include <sofa/defaulttype/DataTypeInfo.h>
using sofa::defaulttype::AbstractTypeInfo;

namespace sofaqtquick {

SofaDataContainerListModel::SofaDataContainerListModel(QObject* parent)
    : QAbstractTableModel(parent),
      m_sofaData(nullptr)
{

}

void SofaDataContainerListModel::setSofaData(sofaqtquick::bindings::SofaData* newSofaData)
{
    if (!newSofaData->rawData()->getValueTypeInfo()->Container())
        msg_error("SofaDataContainerListModel") << "Trying to construct a table model from a non-container type.";
    else
        m_sofaData = newSofaData;
}


int	SofaDataContainerListModel::rowCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;
    return m_sofaData->getProperties()["rows"].toInt();
}

int	SofaDataContainerListModel::columnCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;
    return m_sofaData->getProperties()["cols"].toInt();
}


QVariant SofaDataContainerListModel::data(const QModelIndex &index, int role) const
{
    SOFA_UNUSED(role);
    size_t row = size_t(index.row());
    size_t col = size_t(index.column());


    const AbstractTypeInfo* typeinfo = m_sofaData->rawData()->getValueTypeInfo();
    if (typeinfo->Scalar())
        return QVariant::fromValue(typeinfo->getScalarValue(
                                       m_sofaData->rawData()->getValueVoidPtr(),
                                       col * size_t(columnCount()) + row));
    else if (typeinfo->Integer())
        return QVariant::fromValue(typeinfo->getIntegerValue(
                                       m_sofaData->rawData()->getValueVoidPtr(),
                                       col * size_t(columnCount()) + row));
    else
        return  QVariant();
}

QVariant SofaDataContainerListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return QVariant::fromValue<int>(section);
    else
    {
        switch (section)
        {
        case 0:
            return QVariant::fromValue<QString>("x");
        case 1:
            return QVariant::fromValue<QString>("y");
        case 2:
            return QVariant::fromValue<QString>("z");
        case 3:
            if (columnCount() == 4) // consider it to be a quaternion
                return QVariant::fromValue<QString>("w");
            else if (columnCount() == 6) {
                return QVariant::fromValue<QString>("rx");
            }
            else {
                return QVariant::fromValue<int>(section);
            }
        case 4:
            if (columnCount() == 6) // consider it to be a quaternion
                return QVariant::fromValue<QString>("ry");
            else {
                return QVariant::fromValue<int>(section);
            }
        case 5:
            if (columnCount() == 6) // consider it to be a quaternion
                return QVariant::fromValue<QString>("rz");
            else {
                return QVariant::fromValue<int>(section);
            }
        default:
            return QVariant::fromValue<int>(section);
        }
    }
}

}  // namespace sofaqtquick

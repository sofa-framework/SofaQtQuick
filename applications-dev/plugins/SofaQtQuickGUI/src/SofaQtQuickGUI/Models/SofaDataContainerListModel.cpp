#include "SofaDataContainerListModel.h"

#include <sofa/defaulttype/DataTypeInfo.h>
using sofa::defaulttype::AbstractTypeInfo;

namespace sofaqtquick {

SofaDataContainerListModel::SofaDataContainerListModel(QObject* parent)
    : QAbstractTableModel(parent),
      m_sofaData(nullptr),
      m_asGridViewModel(true)
{

}

void SofaDataContainerListModel::insertRow(QVariantList list)
{
    beginInsertRows(QModelIndex(), nRows(), nRows());
    insertRow(0, QModelIndex());
    for (int i = 0 ; i < nCols() ; ++i)
        setData(createIndex(nRows() - 1, 0, nullptr), list.at(i), i+1);
    endInsertRows();
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

   if (m_asGridViewModel)
        return nRows() * nCols();
    return nRows();
}

int	SofaDataContainerListModel::columnCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;

    auto typeinfo = m_sofaData->rawData()->getValueTypeInfo();
    int nbCols = int(typeinfo->BaseType()->size());
    if (nbCols == 1)
        nbCols = int(typeinfo->size());

    return nbCols;
}

QString	SofaDataContainerListModel::getCornerType(const QModelIndex& idx) const
{
    int r = idx.row() / nCols();
    int c = idx.row() % nCols();

    bool top = r == 0;
    bool bottom = r == nRows() - 1;
    bool left = c == 0;
    bool right = c == nCols() - 1;

    bool singleColumn = nCols() == 1;
    bool singleRow = nRows() == 1;

    if (singleColumn && singleRow)
        return "Single";
    if (singleColumn && top)
        return "Top";
    if (singleColumn && bottom)
        return "Bottom";
    if (singleRow && left)
        return "Left";
    if (singleRow && right)
        return "Right";
    if (top && left)
        return "TopLeft";
    if (top && right)
        return "TopRight";
    if (bottom && left)
        return "BottomLeft";
    if (bottom && right)
        return "BottomRight";
    else
        return "Middle";
}

QVariant SofaDataContainerListModel::data(const QModelIndex& index, int role) const
{
    size_t row = 0;
    size_t col = 0;
    if (m_asGridViewModel)
    {
        if (role == CornerRole)
        {
            return getCornerType(index);
        }
        if (role == IndexRole)
        {
            return index;
        }
        // Add Here whatever role you might want to catch in the views

        else if (role != ValueRole)
            return QVariant();
        row = size_t(index.row() / nCols());
        col = size_t(index.row() % nCols());
    }
    else {
        row = size_t(index.row());
        col = size_t(role - 1);
    }


    const AbstractTypeInfo* typeinfo = m_sofaData->rawData()->getValueTypeInfo();

    if (typeinfo->Scalar())
        return QVariant::fromValue(typeinfo->getScalarValue(
                                       m_sofaData->rawData()->getValueVoidPtr(),
                                       row * size_t(nCols()) + col));
    else if (typeinfo->Integer())
        return QVariant::fromValue(typeinfo->getIntegerValue(
                                       m_sofaData->rawData()->getValueVoidPtr(),
                                       row * size_t(nCols()) + col));
    else
        return  QVariant();
}

QVariant SofaDataContainerListModel::headerData(int section, Qt::Orientation orientation, int /*role*/) const
{
    ///  @bmarques TODO: use typename to determine column header titles
    if (orientation == Qt::Vertical) {
        return QVariant::fromValue<int>(section);
    }
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
        default:
            return QVariant::fromValue<int>(section);
        }
    }
}

bool SofaDataContainerListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    size_t row;
    size_t col;

    if (m_asGridViewModel)
    {
        row = size_t(index.row() / nCols());
        col = size_t(index.row() % nCols());
    }
    else {
        row = size_t(index.row());
        col = size_t(role - 1);
    }
    const AbstractTypeInfo* typeinfo = m_sofaData->rawData()->getValueTypeInfo();

    if (typeinfo->Scalar())
        typeinfo->setScalarValue(m_sofaData->rawData()->beginEditVoidPtr(),
                                 row * size_t(nCols()) + col,
                                 value.toReal());
    else if (typeinfo->Integer())
        typeinfo->setScalarValue(m_sofaData->rawData()->beginEditVoidPtr(),
                                 row * size_t(nCols()) + col,
                                 value.toInt());
    else
        return  false;
    emit sofaDataChanged(m_sofaData);
    return true;
}

bool SofaDataContainerListModel::insertRow(int row, const QModelIndex &parent)
{
    SOFA_UNUSED(row);
    SOFA_UNUSED(parent);
    const auto& d = m_sofaData->rawData();
    const AbstractTypeInfo* typeinfo = d->getValueTypeInfo();

    if (typeinfo->FixedSize())
        return false;
    typeinfo->setSize(d->beginEditVoidPtr(), typeinfo->size(d->getValueVoidPtr()) + typeinfo->BaseType()->size());
    emit m_sofaData->valueChanged(m_sofaData->getValue());
    return true;
}

bool SofaDataContainerListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    SOFA_UNUSED(row);
    SOFA_UNUSED(parent);
    const auto& d = m_sofaData->rawData();
    const AbstractTypeInfo* typeinfo = d->getValueTypeInfo();

    if (typeinfo->FixedSize())
        return false;
    typeinfo->setSize(d->beginEditVoidPtr(), typeinfo->size(d->getValueVoidPtr()) + size_t(count) * typeinfo->BaseType()->size());
    return true;
}

bool SofaDataContainerListModel::removeRow(int row, const QModelIndex &parent)
{
    SOFA_UNUSED(row);
    SOFA_UNUSED(parent);
    const auto& d = m_sofaData->rawData();
    const AbstractTypeInfo* typeinfo = d->getValueTypeInfo();

    if (typeinfo->FixedSize())
        return false;
    typeinfo->setSize(d->beginEditVoidPtr(), typeinfo->size(d->getValueVoidPtr()) - typeinfo->BaseType()->size());
    return true;
}

bool SofaDataContainerListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    SOFA_UNUSED(row);
    SOFA_UNUSED(parent);
    const auto& d = m_sofaData->rawData();
    const AbstractTypeInfo* typeinfo = d->getValueTypeInfo();

    if (typeinfo->FixedSize())
        return false;
    typeinfo->setSize(d->beginEditVoidPtr(), typeinfo->size(d->getValueVoidPtr()) - size_t(count) * typeinfo->BaseType()->size());
    return true;
}



QHash<int, QByteArray> SofaDataContainerListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    if (m_asGridViewModel)
    {
        roles[CornerRole] = "corner";
        roles[IndexRole] = "_index";
        roles[ValueRole] = "dataValue";
    }
    else
    {
        for (auto i = 0 ; i < m_sofaData->property("cols").toInt() ; ++i)
        {
            std::string str("c");
            str += std::to_string(i);
            roles[ColumnRole+i] = str.c_str();
        }
    }
    return roles;
}

}  // namespace sofaqtquick

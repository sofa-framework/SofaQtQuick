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
    if (!m_sofaData || !m_sofaData->rawData()) {
        return;
    }
    beginInsertRows(QModelIndex(), nRows(), nRows());
    insertRow(0, QModelIndex());
    endInsertRows();
    for (int i = 0 ; i < nCols() ; ++i)
        if (i > list.size())
            setData(createIndex(nRows() - 1, 0, nullptr), 0, i+1);
        else {
            setData(createIndex(nRows() - 1, 0, nullptr), list.at(i), i+1);
            std::cout << list.at(i).toString().toStdString() << std::endl;
        }
    if (!m_asGridViewModel) {
        /// ugly hack to keep an accurate number of rows when displaying in a QtQuick.Controls 1.X TableView...
        beginRemoveRows(QModelIndex(), nRows() -1, nRows() -1);
        endRemoveRows();
    }
}

void SofaDataContainerListModel::removeLastRow()
{
    if (!m_sofaData || !m_sofaData->rawData()) {
        return;
    }
    beginRemoveRows(QModelIndex(), nRows() -1, nRows() -1);
    removeRow(nRows() - 1);
    endRemoveRows();
}

void SofaDataContainerListModel::setSofaData(sofaqtquick::bindings::SofaData* newSofaData)
{
    if (!newSofaData || !newSofaData->rawData() || !newSofaData->rawData()->getValueTypeInfo()->Container())
    {
        msg_error("SofaDataContainerListModel") << "Trying to construct a table model from a non-container type.";
        return;
    }

    m_sofaData = newSofaData;
    connect(m_sofaData, &SofaData::valueChanged, this, &SofaDataContainerListModel::onBaseDataValueChanged);
}

void SofaDataContainerListModel::onBaseDataValueChanged()
{
    beginResetModel();
    endResetModel();
}

int	SofaDataContainerListModel::rowCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;

    if (!m_sofaData || !m_sofaData->rawData()) return 0;
   if (m_asGridViewModel)
        return nRows() * nCols();
    return nRows();
}

int	SofaDataContainerListModel::columnCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;

    if (!m_sofaData || !m_sofaData->rawData()) return 0;
    auto typeinfo = m_sofaData->rawData()->getValueTypeInfo();
    int nbCols = int(typeinfo->BaseType()->size());
    if (nbCols == 1)
        nbCols = int(typeinfo->size());
    if (!m_asGridViewModel)
        nbCols++;
    return nbCols;
}

QString	SofaDataContainerListModel::getCornerType(const QModelIndex& idx) const
{
    if (!m_sofaData || !m_sofaData->rawData()) return "Middle";

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
    if (!index.isValid() || !m_sofaData || !m_sofaData->rawData())
        return QVariant();
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
        if (role == 0)
        {
            return QVariant::fromValue(index.row());
        }

        col = size_t(role - 1);
    }

    const AbstractTypeInfo* typeinfo = m_sofaData->rawData()->getValueTypeInfo();

    if (typeinfo->Scalar())
    {
        return QVariant::fromValue(typeinfo->getScalarValue(
                                       m_sofaData->rawData()->getValueVoidPtr(),
                                       row * size_t(nCols()) + col));
    }
    else if (typeinfo->Integer())
    {
        return QVariant::fromValue(typeinfo->getIntegerValue(
                                       m_sofaData->rawData()->getValueVoidPtr(),
                                       row * size_t(nCols()) + col));
    }
    else {
        return  QVariant();
    }
}

QVariant SofaDataContainerListModel::headerData(int section, Qt::Orientation orientation, int /*role*/) const
{
    std::vector<QString> labels;

    ///  @bmarques TODO: use typename to determine column header titles
    return QVariant::fromValue<int>(section);
}

bool SofaDataContainerListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    size_t row;
    size_t col;

    if (!m_sofaData || !m_sofaData->rawData()) return false;
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

    if (typeinfo->Scalar()) {
        m_sofaData->_disconnect();
        typeinfo->setScalarValue(m_sofaData->rawData()->beginEditVoidPtr(),
                                 row * size_t(nCols()) + col,
                                 value.toReal());
        m_sofaData->rawData()->endEditVoidPtr();        
        m_sofaData->_connect();
    }
    else if (typeinfo->Integer()) {
        m_sofaData->_disconnect();
        typeinfo->setScalarValue(m_sofaData->rawData()->beginEditVoidPtr(),
                                 row * size_t(nCols()) + col,
                                 value.toInt());
        m_sofaData->rawData()->endEditVoidPtr();
        m_sofaData->_connect();
    }
    else
        return  false;
    m_sofaData->setPersistent(true);
    return true;
}

bool SofaDataContainerListModel::insertRow(int row, const QModelIndex &parent)
{
    SOFA_UNUSED(row);
    SOFA_UNUSED(parent);
    if (!m_sofaData || !m_sofaData->rawData()) return false;
    const auto& d = m_sofaData->rawData();
    const AbstractTypeInfo* typeinfo = d->getValueTypeInfo();

    if (typeinfo->FixedSize())
        return false;
    typeinfo->setSize(d->beginEditVoidPtr(), typeinfo->size(d->getValueVoidPtr()) + typeinfo->BaseType()->size());
    d->endEditVoidPtr();
    m_sofaData->rawData()->setPersistent(true);

    return true;
}

bool SofaDataContainerListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    SOFA_UNUSED(row);
    SOFA_UNUSED(parent);
    if (!m_sofaData || !m_sofaData->rawData()) return false;
    const auto& d = m_sofaData->rawData();
    const AbstractTypeInfo* typeinfo = d->getValueTypeInfo();

    if (typeinfo->FixedSize())
        return false;
    typeinfo->setSize(d->beginEditVoidPtr(), typeinfo->size(d->getValueVoidPtr()) + size_t(count) * typeinfo->BaseType()->size());
    d->endEditVoidPtr();
    m_sofaData->rawData()->setPersistent(true);
    return true;
}

bool SofaDataContainerListModel::removeRow(int row, const QModelIndex &parent)
{
    SOFA_UNUSED(row);
    SOFA_UNUSED(parent);
    if (!m_sofaData || !m_sofaData->rawData()) return false;
    const auto& d = m_sofaData->rawData();
    const AbstractTypeInfo* typeinfo = d->getValueTypeInfo();

    if (typeinfo->FixedSize())
        return false;
    typeinfo->setSize(d->beginEditVoidPtr(), typeinfo->size(d->getValueVoidPtr()) - typeinfo->BaseType()->size());
    d->endEditVoidPtr();
    m_sofaData->rawData()->setPersistent(true);
    return true;
}

bool SofaDataContainerListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    SOFA_UNUSED(row);
    SOFA_UNUSED(parent);
    if (!m_sofaData || !m_sofaData->rawData()) return false;
    const auto& d = m_sofaData->rawData();
    const AbstractTypeInfo* typeinfo = d->getValueTypeInfo();

    if (typeinfo->FixedSize())
        return false;
    typeinfo->setSize(d->beginEditVoidPtr(), typeinfo->size(d->getValueVoidPtr()) - size_t(count) * typeinfo->BaseType()->size());
    d->endEditVoidPtr();
    m_sofaData->rawData()->setPersistent(true);
    return true;
}



QHash<int, QByteArray> SofaDataContainerListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    if (!m_sofaData || !m_sofaData->rawData()) return roles;
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

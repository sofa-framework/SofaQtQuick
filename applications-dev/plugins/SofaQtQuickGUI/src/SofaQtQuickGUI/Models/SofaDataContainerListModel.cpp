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
        return m_sofaData->getProperties()["rows"].toInt() * m_sofaData->getProperties()["cols"].toInt();
    return m_sofaData->getProperties()["rows"].toInt();
}

int	SofaDataContainerListModel::columnCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;

    return m_sofaData->getProperties()["cols"].toInt();
}

QString	SofaDataContainerListModel::getCornerType(const QModelIndex& idx) const
{
    int ncols = m_sofaData->getProperties()["cols"].toInt();
    int nrows = m_sofaData->getProperties()["rows"].toInt();
    int r = idx.row() / ncols;
    int c = idx.row() % ncols;

    bool top = r == 0;
    bool bottom = r == nrows - 1;
    bool left = c == 0;
    bool right = c == ncols - 1;

    bool singleColumn = ncols == 1;
    bool singleRow = nrows == 1;

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
    size_t ncols = size_t(m_sofaData->getProperties()["cols"].toInt());
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
        row = size_t(index.row()) / ncols;
        col = size_t(index.row()) % ncols;
    }
    else {
        row = size_t(index.row());
        col = size_t(role - 1);
    }


    const AbstractTypeInfo* typeinfo = m_sofaData->rawData()->getValueTypeInfo();

    std::cout << row << ":" << col << " = " << typeinfo->getScalarValue(
                     m_sofaData->rawData()->getValueVoidPtr(),
                     row * size_t(ncols) + col) << std::endl;
    if (typeinfo->Scalar())
        return QVariant::fromValue(typeinfo->getScalarValue(
                                       m_sofaData->rawData()->getValueVoidPtr(),
                                       row * size_t(ncols) + col));
    else if (typeinfo->Integer())
        return QVariant::fromValue(typeinfo->getIntegerValue(
                                       m_sofaData->rawData()->getValueVoidPtr(),
                                       row * size_t(ncols) + col));
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
    std::cout << "CHANGING DATA" << std::endl;
    size_t row;
    size_t col;
    size_t ncols = size_t(m_sofaData->getProperties()["cols"].toInt());

    if (m_asGridViewModel)
    {
        std::cout << "index: " << index.row() << ":" << index.column() << std::endl;
        row = size_t(index.row()) / ncols;
        col = size_t(index.row()) % ncols;
    }
    else {
        row = size_t(index.row());
        col = size_t(role - 1);
    }
    const AbstractTypeInfo* typeinfo = m_sofaData->rawData()->getValueTypeInfo();
    std::cout << "SetDATA: " << row << ":" << col << " => " << value.toReal() << std::endl;

    if (typeinfo->Scalar())
        typeinfo->setScalarValue(m_sofaData->rawData()->beginEditVoidPtr(),
                                 row * size_t(ncols) + col,
                                 value.toReal());
    else if (typeinfo->Integer())
        typeinfo->setScalarValue(m_sofaData->rawData()->beginEditVoidPtr(),
                                 row * size_t(ncols) + col,
                                 value.toInt());
    else
        return  false;
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

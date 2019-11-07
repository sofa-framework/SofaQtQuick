#include "CustomInspectorModel.h"

namespace sofaqtquick {

CustomInspectorModel::CustomInspectorModel(QObject *parent)
    : QAbstractListModel(parent),
      m_sofaComponent(nullptr),
      m_dataList(QStringList())
{
}

sofaqtquick::bindings::SofaBase* CustomInspectorModel::sofaComponent() const { return m_sofaComponent; }

QStringList CustomInspectorModel::dataList() const { return m_dataList; }

void CustomInspectorModel::setSofaComponent(sofaqtquick::bindings::SofaBase* newSofaComponent)
{
    m_sofaComponent = newSofaComponent;
    emit sofaComponentChanged(m_sofaComponent);
}

void CustomInspectorModel::setDataList(QStringList newDataList)
{
    m_dataList = newDataList;
    emit dataListChanged(m_dataList);
}

} // namespace sofaqtquick

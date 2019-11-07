#pragma once

#include <QAbstractListModel>
#include <QVariantList>
#include <QAbstractListModel>
#include <SofaQtQuickGUI/Bindings/SofaData.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>

namespace sofaqtquick
{
class CustomInspectorModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DataCompletionRoles {
        DataNameRole = Qt::UserRole + 1,
        DataGroupRole,
        DataTypeRole,
        DataRole
    };
    CustomInspectorModel(QObject* parent = nullptr);



    Q_PROPERTY(sofaqtquick::bindings::SofaBase* sofaComponent
               READ sofaComponent
               WRITE setSofaComponent
               NOTIFY sofaComponentChanged)

    Q_PROPERTY(QStringList dataList
               READ dataList
               WRITE setDataList
               NOTIFY dataListChanged)

    sofaqtquick::bindings::SofaBase* sofaComponent() const;
    QStringList dataList() const;

    void setSofaComponent(sofaqtquick::bindings::SofaBase* newSofaComponent);

    void setDataList(QStringList newDataDict);

signals:
    void sofaComponentChanged(sofaqtquick::bindings::SofaBase* newSofaComponent) const;
    void dataListChanged(QStringList newDataList) const;

private:
    sofaqtquick::bindings::SofaBase* m_sofaComponent;
    QStringList m_dataList;
};

} // namespace sofaqtquick

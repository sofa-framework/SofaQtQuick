#pragma once
#include <QObject>
#include <QAbstractTableModel>

#include <SofaQtQuickGUI/Bindings/SofaData.h>

namespace sofaqtquick {


class SofaDataContainerListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    SofaDataContainerListModel(QObject* parent = nullptr);

public:
    Q_PROPERTY(sofaqtquick::bindings::SofaData* sofaData READ sofaData WRITE setSofaData NOTIFY sofaDataChanged)
    inline sofaqtquick::bindings::SofaData* sofaData() const { return m_sofaData; }
    void setSofaData(sofaqtquick::bindings::SofaData* newSofaData);
signals:
    void sofaDataChanged(sofaqtquick::bindings::SofaData* newSofaData) const;

protected:
    int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    int	columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
private:
    sofaqtquick::bindings::SofaData* m_sofaData;
};

}  // namespace sofaqtquick

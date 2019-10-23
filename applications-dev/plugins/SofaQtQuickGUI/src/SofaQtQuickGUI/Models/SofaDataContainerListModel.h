#pragma once
#include <QObject>
#include <QAbstractListModel>

#include <SofaQtQuickGUI/Bindings/SofaData.h>

namespace sofaqtquick {


class SofaDataContainerListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataContainerRoles {
        CornerRole = Qt::UserRole + 1,
        IndexRole,
        ValueRole,
        ColumnRole
    };

    SofaDataContainerListModel(QObject* parent = nullptr);

public:
    Q_PROPERTY(sofaqtquick::bindings::SofaData* sofaData READ sofaData WRITE setSofaData NOTIFY sofaDataChanged)
    Q_PROPERTY(bool asGridViewModel READ asGridViewModel WRITE setAsGridViewModel NOTIFY asGridViewModelChanged)

    inline int nCols() const { return m_sofaData ? m_sofaData->getProperties()["cols"].toInt() : 0; }
    inline sofaqtquick::bindings::SofaData* sofaData() const { return m_sofaData; }
    inline bool asGridViewModel() const { return m_asGridViewModel; }

    void setSofaData(sofaqtquick::bindings::SofaData* newSofaData);
    void setAsGridViewModel(bool asGridViewModel) { m_asGridViewModel = asGridViewModel; }

signals:
    void sofaDataChanged(sofaqtquick::bindings::SofaData* newSofaData) const;
    void asGridViewModelChanged(bool) const;

protected:
    QHash<int, QByteArray> roleNames() const;
    int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
private:
    sofaqtquick::bindings::SofaData* m_sofaData;
    bool m_asGridViewModel;

    QString getCornerType(const QModelIndex& idx) const;
};

}  // namespace sofaqtquick

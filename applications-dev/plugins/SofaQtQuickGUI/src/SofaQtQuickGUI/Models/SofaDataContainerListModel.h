#pragma once
#include <QAbstractListModel>

#include <SofaQtQuickGUI/Bindings/SofaData.h>

namespace sofaqtquick
{

using sofaqtquick::bindings::SofaData;

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

    Q_INVOKABLE void insertRow(QVariantList list);
    Q_INVOKABLE void removeLastRow();

    inline sofaqtquick::bindings::SofaData* sofaData() const { return m_sofaData; }
    inline bool asGridViewModel() const { return m_asGridViewModel; }

    void setSofaData(sofaqtquick::bindings::SofaData* newSofaData);
    void setAsGridViewModel(bool asGridViewModel) { m_asGridViewModel = asGridViewModel; }

public slots:
    void onBaseDataValueChanged();

signals:
    void sofaDataChanged(sofaqtquick::bindings::SofaData* newSofaData) const;
    void asGridViewModelChanged(bool) const;

protected:
    QHash<int, QByteArray> roleNames() const;
    int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

private:
    sofaqtquick::bindings::SofaData* m_sofaData;
    bool m_asGridViewModel;

    QString getCornerType(const QModelIndex& idx) const;

    /// Returns the number of columns (always considers the data as a 2D array)
    int nCols() const {
        if (!m_sofaData) return 0;

        auto typeinfo = m_sofaData->rawData()->getValueTypeInfo();
        int nbCols = int(typeinfo->BaseType()->size());
        if (nbCols == 1)
            return int(typeinfo->size());
        return nbCols;
    }
    /// Returns the number of rows (always considers the data as a 2D array)
    int nRows() const
    {
        if (!m_sofaData) return 0;
        auto typeinfo = m_sofaData->rawData()->getValueTypeInfo();
        return int(typeinfo->size(m_sofaData->rawData()->getValueVoidPtr()) / size_t(nCols()));
    }
};

}  // namespace sofaqtquick

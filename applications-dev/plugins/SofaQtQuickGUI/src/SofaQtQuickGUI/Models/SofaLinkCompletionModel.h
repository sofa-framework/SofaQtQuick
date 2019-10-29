#ifndef SOFALINKCOMPLETIONMODEL_H
#define SOFALINKCOMPLETIONMODEL_H

#include <QAbstractListModel>
#include <SofaQtQuickGUI/Bindings/SofaData.h>

namespace sofa::core::objectmodel {
class BaseNode;
}

namespace sofaqtquick {


class SofaLinkCompletionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DataCompletionRoles {
        CornerRole = Qt::UserRole + 1,
        CompletionRole,
        NameRole,
        HelpRole,
    };
    SofaLinkCompletionModel(QObject* parent = nullptr)
        : QAbstractListModel(parent),
          m_sofaData(nullptr),
          m_linkPath("")
    {}

    Q_PROPERTY(sofaqtquick::bindings::SofaData* sofaData READ sofaData WRITE setSofaData NOTIFY sofaDataChanged)
    Q_PROPERTY(QString linkPath READ getLinkPath WRITE setLinkPath NOTIFY linkPathChanged)

    inline sofaqtquick::bindings::SofaData* sofaData() const { return m_sofaData; }
    void setSofaData(sofaqtquick::bindings::SofaData* newSofaData);

    inline QString getLinkPath() const { return m_linkPath; }
    void setLinkPath(QString newlinkPath);

signals:
    void sofaDataChanged(sofaqtquick::bindings::SofaData* newSofaData) const;
    void linkPathChanged(QString newlinkPath) const;

protected:
    int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

private:
    sofaqtquick::bindings::SofaData* m_sofaData;
    QString m_linkPath;

    QStringList m_modelText;
    QStringList m_modelName;
    QStringList m_modelHelp;



    sofa::core::objectmodel::BaseNode* m_relativeRoot;
    sofa::core::objectmodel::BaseNode* m_absoluteRoot;


    bool isValid(const QString& path) const;
    sofa::core::objectmodel::Base* getLastValidObject(QString& );
    void updateModel();

};

}
#endif // SOFALINKCOMPLETIONMODEL_H

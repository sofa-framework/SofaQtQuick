#pragma once

#include <QAbstractListModel>
#include <QList>

#include <sofa/core/objectmodel/Base.h>
#include <SofaQtQuickGUI/Bindings/SofaCamera.h>

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/SofaBaseScene.h>
#include "SofaSceneListModel.h"

class QTimer;
class QVector3D;

namespace sofaqtquick
{
using sofa::component::visualmodel::BaseCamera;

/// \class A Model allowing us to show a list of sofa data belonging to a specific sofa component in a ListView
class SofaCameraListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    SofaCameraListModel(QObject* parent = nullptr);
    ~SofaCameraListModel();

public:
    Q_PROPERTY(sofaqtquick::SofaBaseScene* sofaScene READ sofaScene WRITE setSofaScene NOTIFY sofaSceneChanged)

public:
    sofaqtquick::SofaBaseScene* sofaScene() const;
    void setSofaScene(sofaqtquick::SofaBaseScene* newSofaScene);

public slots:
    Q_INVOKABLE void update(bool reset = false);
    Q_INVOKABLE void forceUpdate();

protected:
    int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;

signals:
    void sofaSceneChanged(sofaqtquick::SofaBaseScene* newSofaComponent) const;

private:
    enum {
        NameRole = Qt::UserRole + 1,
        ValueRole
    };

private:
    sofaqtquick::SofaBaseScene*  m_sofaScene;
    std::vector<BaseCamera*> m_cameras;
};

}  // namespace sofaqtquick

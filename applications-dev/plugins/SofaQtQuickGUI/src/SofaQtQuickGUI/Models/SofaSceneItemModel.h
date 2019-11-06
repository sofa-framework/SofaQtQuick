/*******************************************************************
Copyright 2018, CNRS

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/*******************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/
#ifndef SOFAQTQUICK_MODELS_SOFASCENEITEMMODEL_H
#define SOFAQTQUICK_MODELS_SOFASCENEITEMMODEL_H

#include <QAbstractItemModel>
#include <QTimer>
#include <sofa/simulation/MutationListener.h>

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/SofaScene.h>
#include <SofaQtQuickGUI/Bindings/SofaData.h>
#include <thread>
#include <mutex>

#include <sofa/core/DataTracker.h>

namespace sofa
{

namespace qtquick
{

namespace _sofasceneitemmodel_
{
    using sofa::core::DataTracker;
    using sofa::simulation::MutationListener ;
    using sofa::simulation::Node ;

/// \class A Proxying Model of the sofa scene graph allowing us to show the graph in a TreeView
/// The data are not duplicated but gathered directly from the underlying Sofa Scene.
class SofaSceneItemModel : public QAbstractItemModel, private MutationListener
{
    Q_OBJECT

public:
    Q_PROPERTY(sofa::qtquick::SofaScene* sofaScene READ sofaScene WRITE setSofaScene NOTIFY sofaSceneChanged)

    ///@brief Returns the sofa component corresponding to the provided index.
    Q_INVOKABLE sofaqtquick::bindings::SofaBase* getBaseFromIndex(const QModelIndex& index) const;
    Q_INVOKABLE QModelIndex getIndexFromBase(sofaqtquick::bindings::SofaBase* index) const;


    SofaSceneItemModel(QObject* parent = nullptr);
    ~SofaSceneItemModel() override;

    SofaScene* sofaScene() const;
    void setSofaScene(SofaScene* newScene);

    /// The following methods are inherited from QAbstractItemModel.
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const override ;
    virtual QModelIndex parent(const QModelIndex &index) const override ;
    virtual bool hasChildren(const QModelIndex &index) const override;
    virtual int rowCount(const QModelIndex &parent) const override ;
    virtual int columnCount(const QModelIndex &parent) const override ;
    virtual QVariant data(const QModelIndex &index, int role) const override ;
    virtual QHash<int, QByteArray> roleNames() const override;

    enum class Roles
    {
        Name = Qt::UserRole + 1,
        TypeName,
        ShortName,
        IsNode,
        IsMultiParent,
        HasMultiParent,
        IsEnabled,
        Status,
        Row
    };

signals:
    void sofaSceneChanged();
    void modelHasReset();
    void resetRequired();

public slots:
    void handleRootNodeChange();
    void onResetRequired();
    void onTimeOutModelRefresh();

protected:
    /// The following function are inhereted from MutationLister, they are called when there is
    /// change in the scene this model is exposing. When called this function is function is in
    /// charge of notifying the cbasehange using the QAbstractItemModel.
    void onBeginAddChild(Node* parent, Node* child) override;
    void onEndAddChild(Node* parent, Node* child) override;
    void onBeginRemoveChild(Node* parent, Node* child) override;
    void onEndRemoveChild(Node* parent, Node* child) override;

    void onBeginAddObject(Node* parent, core::objectmodel::BaseObject* obj) override;
    void onEndAddObject(Node* parent, core::objectmodel::BaseObject* obj) override;
    void onBeginRemoveObject(Node* parent, core::objectmodel::BaseObject* obj) override;
    void onEndRemoveObject(Node* parent, core::objectmodel::BaseObject* obj) override;
    void emitAllChanges(Node* node);

    /// Returns the ndex associated to the given node.
    /// If the node parameter is nullptr returns an invalid model index.
    QModelIndex index(simulation::Node *node) const ;
    QModelIndex index(simulation::Node *node, sofa::core::objectmodel::BaseObject* obj) const ;

    SofaScene*                      m_scene {nullptr};
    sofa::simulation::Node::SPtr    m_root  {nullptr};

private:
    void modelRefreshThread();
    void removeNodeContent(Node* node);
    void addNodeContent(Node* node);

    QTimer m_refreshModel;

    std::mutex m_lock;
    std::thread m_thread;
    bool m_isRefreshThreadRunning;
    bool m_needsRefresh;

    DataTracker m_dataTracker;
};

} /// namespace _sofasceneitemmodel_
    using _sofasceneitemmodel_::SofaSceneItemModel ;
} /// namespace qtquick

} /// namespace sofa

#endif /// SOFAQTQUICK_MODELS_SOFASCENEITEMMODEL_H

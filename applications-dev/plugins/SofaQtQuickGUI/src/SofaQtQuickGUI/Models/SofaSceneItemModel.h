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
#include <sofa/simulation/MutationListener.h>

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/SofaScene.h>

namespace sofa
{

namespace qtquick
{

namespace _sofasceneitemmodel_
{
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
    Q_INVOKABLE sofa::qtquick::SofaComponent* getComponentFromIndex(const QModelIndex& index) const;

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
        IsNode,
        IsMultiParent,
        HasMultiParent,
        IsEnabled,
        Row
    };

signals:
    void sofaSceneChanged();

protected slots:
    void handleRootNodeChange();

protected:
    /// The following function are inhereted from MutationLister, they are called when there is
    /// change in the scene this model is exposing. When called this function is function is in
    /// charge of notifying the change using the QAbstractItemModel.
//    bool frozen;
    void onStepBegin(Node* root) override;
    void onStepEnd(Node* root) override;

    void onAddChildBegin(Node* parent, Node* child) override;
    void onAddChildEnd(Node* parent, Node* child) override;
    void onRemoveChildBegin(Node* parent, Node* child) override;
    void onRemoveChildEnd(Node* parent, Node* child) override;

    void onAddObjectBegin(Node* parent, core::objectmodel::BaseObject* obj) override;
    void onAddObjectEnd(Node* parent, core::objectmodel::BaseObject* obj) override;
    void onRemoveObjectBegin(Node* parent, core::objectmodel::BaseObject* obj) override;
    void onRemoveObjectEnd(Node* parent, core::objectmodel::BaseObject* obj) override;

    /// Returns the ndex associated to the given node.
    /// If the node parameter is nullptr returns an invalid model index.
    QModelIndex index(simulation::Node *node) const ;
    QModelIndex index(simulation::Node *node, sofa::core::objectmodel::BaseObject* obj) const ;

    SofaScene*                      m_scene {nullptr};
    sofa::simulation::Node::SPtr    m_root  {nullptr};

private:
    void removeNodeContent(Node* node);
    void addNodeContent(Node* node);
};

} /// namespace _sofasceneitemmodel_
    using _sofasceneitemmodel_::SofaSceneItemModel ;
} /// namespace qtquick

} /// namespace sofa

#endif // SOFAQTQUICK_MODELS_SOFASCENEITEMMODEL_H

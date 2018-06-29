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

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
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
    Q_PROPERTY(sofa::qtquick::SofaScene* sofaScene READ sofaScene WRITE setSofaScene)

public:
    SofaSceneItemModel(QObject* parent = 0);
    ~SofaSceneItemModel();

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

protected:
    /// The following function are inhereted from MutationLister, they are called when there is
    /// change in the scene this model is exposing. When called this function is function is in
    /// charge of notifying the change using the QAbstractItemModel.
    virtual void addChild(Node* parent, Node* child) override;
    virtual void addChildDone(Node* parent, Node* child) override;

    //virtual void removeChild(Node* parent, Node* child) override;
    //virtual void addObject(Node* parent, sofa::core::objectmodel::BaseObject* object) override;
    //virtual void removeObject(Node* parent, sofa::core::objectmodel::BaseObject* object)override;

    /// Returns the ndex associated to the given node.
    /// If the node parameter is nullptr returns an invalid model index.
    QModelIndex index(simulation::Node *node) const ;

    SofaScene*                      m_scene {nullptr};
};

} /// namespace _sofasceneitemmodel_
    using _sofasceneitemmodel_::SofaSceneItemModel ;
} /// namespace qtquick

} /// namespace sofa

#endif // SOFAQTQUICK_MODELS_SOFASCENEITEMMODEL_H

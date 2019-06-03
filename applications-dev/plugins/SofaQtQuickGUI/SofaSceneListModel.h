/*
Copyright 2015, Anatoscope

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
*/

#ifndef SOFASCENELISTMODEL_H
#define SOFASCENELISTMODEL_H

#include "SofaQtQuickGUI.h"
#include "SofaScene.h"

#include <sofa/simulation/MutationListener.h>

#include <QAbstractListModel>
#include <QQmlParserStatus>

namespace sofa
{

namespace qtquick
{

/// \class A Model of the sofa scene graph allowing us to show the graph in a ListView
class SofaSceneListModel : public QAbstractListModel, private sofa::simulation::MutationListener
{
    Q_OBJECT

public:
    SofaSceneListModel(QObject* parent = 0);
    ~SofaSceneListModel();

    Q_INVOKABLE void update();

    Q_ENUMS(Visibility)
    enum Visibility {
        Visible     = 0,
        Collapsed   = 1,
        Hidden      = 2,
        Disabled    = 4
    };

public slots:
    void clear();

public:
    Q_PROPERTY(sofa::qtquick::SofaScene* sofaScene READ sofaScene WRITE setSofaScene NOTIFY sofaSceneChanged)

public:
    SofaScene* sofaScene() const		{return mySofaScene;}
    void setSofaScene(SofaScene* newScene);

protected:
    Q_INVOKABLE int computeModelRow(int itemIndex) const;
    Q_INVOKABLE int computeItemRow(int modelIndex) const;
    int computeItemRow(const QModelIndex& index) const;
    int	rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE QVariant get(int row) const;

    Q_INVOKABLE void setDisabled(int modelRow, bool disabled);
    Q_INVOKABLE void setCollapsed(int modelRow, bool value);

    Q_INVOKABLE sofa::qtquick::SofaComponent* getComponentById(int row) const;
    Q_INVOKABLE int getComponentId(sofa::qtquick::SofaComponent*) const;

    Q_INVOKABLE QList<int> computeFilteredRows(const QString& filter) const;

    void handleSceneChange(SofaScene* newScene);

signals:
    void sofaSceneChanged(sofa::qtquick::SofaScene* newScene);

protected:
    void addChild(sofa::simulation::Node* parent, sofa::simulation::Node* child);
    void removeChild(sofa::simulation::Node* parent, sofa::simulation::Node* child);
    void addObject(sofa::simulation::Node* parent, sofa::core::objectmodel::BaseObject* object);
    void removeObject(sofa::simulation::Node* parent, sofa::core::objectmodel::BaseObject* object);

private:
    enum {
        NameRole = Qt::UserRole + 1,
        MultiParentRole,
        DepthRole,
        VisibilityRole,
        TypeRole,
        IsNodeRole,
        CollapsibleRole
    };

    struct Item
    {
        Item() :
            parent(0),
            children(0),
            multiparent(false),
            depth(0),
            visibility(0),
            base(0),
            object(0),
            node(0)
        {

        }

        Item*                                   parent;
        QVector<Item*>                          children;
        bool                                    multiparent;
        int                                     depth;
        int                                     visibility;

        sofa::core::objectmodel::Base*          base;
        sofa::core::objectmodel::BaseObject*    object;
        sofa::core::objectmodel::BaseNode*      node;
    };

    Item buildNodeItem(Item* parent, sofa::core::objectmodel::BaseNode* node) const;
    Item buildObjectItem(Item* parent, sofa::core::objectmodel::BaseObject* object) const;

private:
    bool isAncestor(Item* ancestor, Item* child);

private:
    QList<Item>                     myItems;
    SofaScene*                      mySofaScene;
};

}

}

#endif // SOFASCENELISTMODEL_H

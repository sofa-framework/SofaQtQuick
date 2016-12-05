/*
Author: damien.marchal@univ-lille1.fr
Copyright 2016 CNRS,

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

#ifndef SOFAINSPECTORDATALISTMODEL_H
#define SOFAINSPECTORDATALISTMODEL_H

#include "SofaQtQuickGUI.h"
#include "SofaScene.h"
#include "SofaSceneListModel.h"

#include <sofa/core/objectmodel/Base.h>

#include <QAbstractListModel>
#include <QList>

class QTimer;
class QVector3D;

namespace sofa
{

namespace qtquick
{

class ItemBase {
public:
    ItemBase(){ m_visible = true; }
    virtual ~ItemBase(){}
    bool isVisible(){ return m_visible; }
    void setVisibility(bool theNewState){ m_visible = theNewState; }

    QString         m_name          {""};
    bool m_visible                  {true};
};

class Item : public QObject, public ItemBase
{
    Q_OBJECT

    Q_PROPERTY(QString m_name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant m_data READ getData WRITE setData NOTIFY dataChanged)

public:
    Item(const QString& name, const QVariant& data, int type, bool visible) :

        m_type(type),
        m_data(data)
        { m_visible=visible;
          m_name=name;
        }
    virtual ~Item(){}

    int         m_type;
    QVariant    m_data;

    QString getName() { return m_name ; }
    void setName(QString& s){ m_name=s; }

    QVariant getData() { return m_data ; }
    void setData(QVariant& s){ m_data=s; }

signals:
    void nameChanged(QString*) const ;
    void dataChanged(QVariant*) const ;
};

class ItemGroup : public ItemBase
{
public:
    ItemGroup() {}
    ItemGroup(const QString& name)  { m_name = name; }
    virtual ~ItemGroup(){
        for(Item* item : m_children)
            delete item ;
        m_children.clear();
    }

    QList<Item*>    m_children;
    int             m_order         {0};
};

/// \class A Model allowing us to show a list of sofa data belonging to
/// a specific sofa component in a ListView
class SofaInspectorDataListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    SofaInspectorDataListModel(QObject* parent = 0);
    ~SofaInspectorDataListModel();

    Q_INVOKABLE void update();
    Q_PROPERTY(sofa::qtquick::SofaComponent* sofaSelectedComponent
               READ sofaSelectedComponent
               WRITE setSofaSelectedComponent
               NOTIFY sofaSelectedComponentChanged)

     Q_INVOKABLE QModelIndex index(int row, int col,
                                   const QModelIndex& parent) const ;
     Q_INVOKABLE int rowCount(int row)  ;


    enum Type {
        SofaDataType = 5,
        SofaLinkType,
        InfoType,
        LogType,
        GroupType,
    };
    Q_ENUMS(Type);

    enum Visibility {
        Expanded = 1,
        Collapsed,
        Hidden
    };
    Q_ENUMS(Visibility)

    SofaComponent* sofaSelectedComponent() const {return m_selectedsofacomponent;}
    void setSofaSelectedComponent(SofaComponent* newSofaComponent);

public slots:
    void handleDataHasChanged() ;

signals:
    void sofaSelectedComponentChanged(SofaComponent* newSofaComponent) const;
    void sofaDataHasChanged() ;

protected:
    QModelIndex parent ( const QModelIndex & index ) const ;
    int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    int	columnCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;

    Q_INVOKABLE sofa::qtquick::SofaData* getDataById(int parent, int child) const;
    Q_INVOKABLE bool isGroupVisible(int id) const ;
    Q_INVOKABLE bool isItemVisible(int parent, int child) const ;

    Q_INVOKABLE void setVisibility(int groupid, bool visibility) ;
    Q_INVOKABLE void setOrdering(int groupid, int idx) ;
    Q_INVOKABLE void setVisibility(int groupid, int itemid, bool visibility) ;

private:
    enum {
        NameRole = Qt::UserRole + 1,
        TypeRole,
        ValueRole,
        VisibilityRole,
    };

    ItemGroup* findOrCreateGroup(const QString& groupname) ;

    QList<ItemGroup*>             m_groups;
    QMap<QString, int>            m_nameindex ;
    SofaComponent*                m_selectedsofacomponent;
};

}

}

#endif // SOFAINSPECTORDATALISTMODEL_H

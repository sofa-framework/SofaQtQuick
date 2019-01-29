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

#ifndef SOFADATALISTMODEL_H
#define SOFADATALISTMODEL_H

#include <QAbstractListModel>
#include <QList>

#include <sofa/core/objectmodel/Base.h>

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
#include <SofaQtQuickGUI/SofaScene.h>
#include "SofaSceneListModel.h"

class QTimer;
class QVector3D;

namespace sofa
{

namespace qtquick
{

/// \class A Model allowing us to show a list of sofa data belonging to a specific sofa component in a ListView
class SofaDataListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    SofaDataListModel(QObject* parent = 0);
    ~SofaDataListModel();

    Q_INVOKABLE void update();

public:
    Q_PROPERTY(sofa::qtquick::SofaComponent* sofaComponent READ sofaComponent WRITE setSofaComponent NOTIFY sofaComponentChanged)

    enum Type {
        SofaDataType = 0,
        SofaLinkType,
		LogType,
        InfoType,
    };
    Q_ENUMS(Type);

public:
    SofaComponent* sofaComponent() const		{return mySofaComponent;}
    void setSofaComponent(SofaComponent* newSofaComponent);

protected:
    int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;

    Q_INVOKABLE sofa::qtquick::SofaData* getDataById(int row) const;
signals:
    void sofaComponentChanged(SofaComponent* newSofaComponent) const;

private:
    enum {
        NameRole = Qt::UserRole + 1,
        GroupRole,
        TypeRole,
        ValueRole
    };

    struct Item
    {
        Item() :
            name(),
            group(),
            type(),
            data(0)
        {

        }

        QString     name;
        QString     group;
        int         type;
        QVariant    data;
    };

    Item buildItem(sofa::core::objectmodel::BaseData* data) const;
    Item buildItem(sofa::core::objectmodel::BaseLink* link) const;
    Item buildItem(const QString& name, const QString& group, const QString& value) const; // readonly

private:
    QList<Item>             myItems;
    int                     myUpdatedCount;
    mutable SofaComponent*  mySofaComponent;

};

}

}

#endif // SOFADATALISTMODEL_H

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

class SofaDataListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    SofaDataListModel(QObject* parent = 0);
    ~SofaDataListModel();

    Q_INVOKABLE void update();

public:
    Q_PROPERTY(sofa::qtquick::SofaComponent* sofaComponent READ sofaComponent WRITE setSofaComponent NOTIFY sofaComponentChanged)

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
        ValueRole
    };

    struct Item
    {
        Item() :
            data(0)
        {

        }

        sofa::core::objectmodel::BaseData*      data;
    };

    Item buildSofaDataItem(sofa::core::objectmodel::BaseData* data) const;

private:
    QList<Item>             myItems;
    int                     myUpdatedCount;
    mutable SofaComponent*  mySofaComponent;

};

}

}

#endif // SOFADATALISTMODEL_H

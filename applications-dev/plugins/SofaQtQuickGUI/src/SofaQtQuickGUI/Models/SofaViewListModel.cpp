/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/


#include "SofaViewListModel.h"
#include <sofa/helper/logging/Messaging.h>

namespace sofa
{

namespace qtquick
{

SofaViewListModel::SofaViewListModel(QObject* parent) :
    QAbstractListModel(parent),
    myItems()
{
    update();
}

SofaViewListModel::~SofaViewListModel()
{

}

void SofaViewListModel::update()
{
    beginResetModel();

    myItems.clear();
    myItems.append(Item("Set1","File1"));
    myItems.append(Item("Set2","File2"));

    endResetModel();
}

int	SofaViewListModel::rowCount(const QModelIndex & /*parent*/) const
{
    return myItems.size();
}

QVariant SofaViewListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
    {
        msg_error("SofaQtQuickGUI") << "SofaViewListModel::data  Invalid index";
        return QVariant("");
    }

    if(index.row() >= myItems.size())
        return QVariant("");

    const Item& item = myItems[index.row()];

    switch(static_cast<Role>(role))
    {
    case Role::Name:
        return QVariant::fromValue(item.name);
    case Role::FilePath:
        return QVariant::fromValue(item.filePath);
    }

    return QVariant("INVALID");
}

QHash<int,QByteArray> SofaViewListModel::roleNames() const
{
    QHash<int,QByteArray> roles;

    roles[static_cast<int>(Role::Name)]         = "name";
    roles[static_cast<int>(Role::FilePath)]     = "filePath";

    return roles;
}

}

}

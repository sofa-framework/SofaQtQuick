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

#ifndef RUNSOFA_SOFAVIEWLISTMODEL_H
#define RUNSOFA_SOFAVIEWLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

namespace sofa
{

namespace qtquick
{

/// \class A Model containing the name/filePath for the view files that are
/// currently in the view directory.
class SofaViewListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    SofaViewListModel(QObject* parent = nullptr);
    ~SofaViewListModel();

    Q_INVOKABLE void update();

protected:
    int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;

private:
    enum class Role {
        Name = Qt::UserRole + 1,
        FilePath
    };

    struct Item
    {
        Item(const QString& a_name, const QString& a_filePath) :
            name(a_name),
            filePath(a_filePath)
        {

        }

        QString     name;
        QString     filePath;
    };

private:
    QList<Item>             myItems;
};

}

}

#endif // RUNSOFA_SOFAVIEWLISTMODEL_H

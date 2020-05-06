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

#ifndef SOFASCENEITEMPROXY_H
#define SOFASCENEITEMPROXY_H

#include <QSortFilterProxyModel>
#include "SofaSceneItemModel.h"

namespace sofaqtquick
{

/// \class A Model of the sofa scene graph allowing us to show the graph in a ListView
class SofaSceneItemProxy : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    Q_INVOKABLE void flipComponentVisibility(QModelIndex index);
    Q_INVOKABLE void showOnlyNodes(bool value);

    Q_PROPERTY(QAbstractItemModel* model READ sourceModel WRITE setSourceModel)

public:

    void setSourceModel(QAbstractItemModel* model) override
    {
        connect((SofaSceneItemModel*)model, &SofaSceneItemModel::modelHasReset, this, &SofaSceneItemProxy::modelHasReset);
        QSortFilterProxyModel::setSourceModel(model);
    }

    SofaSceneItemProxy(QObject* parent = nullptr);
    virtual ~SofaSceneItemProxy()override ;

    /// Inherited from QSortFilterProxyModel by overriding it we can implement
    /// custom filtering rules.
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

signals:
    void modelHasReset();
private:
    QHash<QModelIndex, bool> m_filters ;
    bool                     m_showOnlyNodes {false};
};

}  // namespace sofaqtquick

#endif // SOFASCENEITEMPROXY_H

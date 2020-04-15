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

#include "SofaSceneItemProxy.h"
#include <QDebug>

namespace sofaqtquick
{

SofaSceneItemProxy::SofaSceneItemProxy(QObject* parent) :
    QSortFilterProxyModel(parent)
{
    setFilterKeyColumn(0);
    setRecursiveFilteringEnabled(true);
    setDynamicSortFilter(true);
}


bool SofaSceneItemProxy::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    if (sourceModel() == nullptr)
        return false;
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent) ;

    /// If requested we only show the nodes in the graph.
    if(m_showOnlyNodes)
    {
        /// The data() for Role::iSNode is returning the adequate information.
        /// the implementation is very simple and we can probably accelerate it a bit
        /// by using more "bare metal" object instead index() & data() function.
        return sourceModel()->data(sourceIndex,
                                   static_cast<int>(SofaSceneItemModel::Roles::IsNode)).toBool();
    }

    if(m_filters.contains(sourceParent))
    {
        qDebug() << "Filter " << sourceModel()->data(sourceIndex,
                                                     static_cast<int>(SofaSceneItemModel::Roles::IsNode)).toBool();
        return sourceModel()->data(sourceIndex,
                                   static_cast<int>(SofaSceneItemModel::Roles::IsNode)).toBool();
    }

    return true;
}

SofaSceneItemProxy::~SofaSceneItemProxy()
{

}

void SofaSceneItemProxy::flipComponentVisibility(const QModelIndex index)
{
    QModelIndex srcindex = mapToSource(index);
    if(!m_filters.contains(srcindex))
       m_filters.insert(srcindex, false);
    else
       m_filters.remove(srcindex);

    invalidateFilter();
}

void SofaSceneItemProxy::showOnlyNodes(bool value)
{
    m_showOnlyNodes=value;
    invalidateFilter();
}

} ///namespace sofaqtquick


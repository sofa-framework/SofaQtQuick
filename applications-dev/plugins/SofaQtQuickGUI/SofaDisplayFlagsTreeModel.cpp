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

#include "SofaDisplayFlagsTreeModel.h"

#include <sofa/helper/accessor.h>
#include <SofaBaseVisual/VisualStyle.h>
#include <sofa/core/visual/DisplayFlags.h>

#include <QStack>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

using namespace sofa::helper;
using namespace sofa::core::visual;
using namespace sofa::core::objectmodel;
using namespace sofa::component::visualmodel;

SofaDisplayFlagsTreeModel::SofaDisplayFlagsTreeModel(QObject* parent) : QAbstractItemModel(parent),
    myDisplayFlagsData(nullptr),
    myFlags(HideAll),
    myRootItem(nullptr)
{
    setupTree();

    connect(this, SIGNAL(displayFlagsDataChanged(SofaData*)), this, SLOT(download()));
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(upload()));
}

SofaDisplayFlagsTreeModel::~SofaDisplayFlagsTreeModel()
{
    delete myRootItem;
}

int SofaDisplayFlagsTreeModel::state(const QModelIndex &index)
{
    if(!index.isValid())
        return -1;

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    return item->state();
}

void SofaDisplayFlagsTreeModel::setEnabled(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    if(item->enabled())
        return;

    item->setEnabled();

    dataChanged(index, index);
}

void SofaDisplayFlagsTreeModel::setDisabled(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    if(item->disabled())
        return;

    item->setDisabled();

    dataChanged(index, index);
}

void SofaDisplayFlagsTreeModel::download()
{
    if(!myDisplayFlagsData)
        return;

    const Data<DisplayFlags>* displayFlags = dynamic_cast<Data<DisplayFlags>*>(myDisplayFlagsData->data());
    if(!displayFlags)
        return;

    ReadAccessor<Data<DisplayFlags> > displayFlagsAccessor(displayFlags);
    myFlags = HideAll;

    myFlags |= displayFlagsAccessor->getShowVisualModels() ? ShowVisualModels : 0;
    myFlags |= displayFlagsAccessor->getShowBehaviorModels() ? ShowBehaviorModels : 0;
    myFlags |= displayFlagsAccessor->getShowForceFields() ? ShowForceFields : 0;
    myFlags |= displayFlagsAccessor->getShowInteractionForceFields() ? ShowInteractionForceFields : 0;
    myFlags |= displayFlagsAccessor->getShowCollisionModels() ? ShowCollisionModels : 0;
    myFlags |= displayFlagsAccessor->getShowBoundingCollisionModels() ? ShowBoundingCollisionsModels : 0;
    myFlags |= displayFlagsAccessor->getShowMappings() ? ShowMappings : 0;
    myFlags |= displayFlagsAccessor->getShowMechanicalMappings() ? ShowMechanicalMappings : 0;
    //myFlags |= displayFlagsAccessor->getShowAdvancedRendering() ? ShowAdvancedRendering : 0;
    myFlags |= displayFlagsAccessor->getShowWireFrame() ? ShowWireframe : 0;
    myFlags |= displayFlagsAccessor->getShowNormals() ? ShowNormals : 0;

    myRootItem->download();

    dataChanged(QModelIndex(), QModelIndex());
}

void SofaDisplayFlagsTreeModel::upload()
{
    if(!myDisplayFlagsData)
        return;

    Data<DisplayFlags>* displayFlags = dynamic_cast<Data<DisplayFlags>*>(myDisplayFlagsData->data());
    if(!displayFlags)
        return;

    WriteAccessor<Data<DisplayFlags> > displayFlagsAccessor(*displayFlags);
    displayFlagsAccessor->setShowAll(false);

    displayFlagsAccessor->setShowVisualModels(myFlags & ShowVisualModels);
    displayFlagsAccessor->setShowBehaviorModels(myFlags & ShowBehaviorModels);
    displayFlagsAccessor->setShowForceFields(myFlags & ShowForceFields);
    displayFlagsAccessor->setShowInteractionForceFields(myFlags & ShowInteractionForceFields);
    displayFlagsAccessor->setShowCollisionModels(myFlags & ShowCollisionModels);
    displayFlagsAccessor->setShowBoundingCollisionModels(myFlags & ShowBoundingCollisionsModels);
    displayFlagsAccessor->setShowMappings(myFlags & ShowMappings);
    displayFlagsAccessor->setShowMechanicalMappings(myFlags & ShowMechanicalMappings);

    //displayFlagsAccessor->setShowAdvancedRendering(myFlags & ShowAdvancedRendering);
    displayFlagsAccessor->setShowWireFrame(myFlags & ShowWireframe);
    displayFlagsAccessor->setShowNormals(myFlags & ShowNormals);
}

SofaData* SofaDisplayFlagsTreeModel::displayFlagsData() const
{
    return myDisplayFlagsData;
}

void SofaDisplayFlagsTreeModel::setDisplayFlagsData(SofaData* newDisplayFlagsData)
{
    if(newDisplayFlagsData == myDisplayFlagsData)
        return;

    myDisplayFlagsData = nullptr;
    if(newDisplayFlagsData)
        myDisplayFlagsData = new SofaData(*newDisplayFlagsData);

    displayFlagsDataChanged(newDisplayFlagsData);
}

void SofaDisplayFlagsTreeModel::setupTree()
{
    delete myRootItem;

    myRootItem = TreeItem::createRoot("");
    {
        TreeItem* allItem = myRootItem->createChild("All");
        {
            TreeItem* visualItem = allItem->createChild("Visual");
            {
                visualItem->createChild("Visual Models", this, ShowVisualModels);
            }

            TreeItem* behaviorItem = allItem->createChild("Behavior");
            {
                behaviorItem->createChild("Behavior Model", this, ShowBehaviorModels);
                behaviorItem->createChild("Force Fields", this, ShowForceFields);
                behaviorItem->createChild("Interactions", this, ShowInteractionForceFields);
            }

            TreeItem* collisionItem = allItem->createChild("Collision");
            {
                collisionItem->createChild("Collision Models", this, ShowCollisionModels);
                collisionItem->createChild("Bounding Trees", this, ShowBoundingCollisionsModels);
            }

            TreeItem* mappingItem = allItem->createChild("Mapping");
            {
                mappingItem->createChild("Visual Mappings", this, ShowMappings);
                mappingItem->createChild("Mechanical Mappings", this, ShowMechanicalMappings);
            }

            TreeItem* optionsItem = allItem->createChild("Options");
            {
                optionsItem->createChild("Advanced Rendering", this, ShowAdvancedRendering);
                optionsItem->createChild("Wireframe", this, ShowWireframe);
                optionsItem->createChild("Normals", this, ShowNormals);
            }
        }
    }
}

QModelIndex SofaDisplayFlagsTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem* parentItem = myRootItem;
    if(parent.isValid())
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex SofaDisplayFlagsTreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if(parentItem == myRootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SofaDisplayFlagsTreeModel::rowCount(const QModelIndex &parent) const
{
    if(parent.column() > 0)
        return 0;

    TreeItem* parentItem = myRootItem;
    if(parent.isValid())
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int SofaDisplayFlagsTreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

//    if(parent.isValid())
//        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
//    else
//        return myRootItem->columnCount();

    return 1;
}

QVariant SofaDisplayFlagsTreeModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    if(!index.isValid())
        return QVariant();

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

    return item->name();
}

QHash<int,QByteArray> SofaDisplayFlagsTreeModel::roleNames() const
{
    QHash<int,QByteArray> roles;

    roles[NameRole] = "Name";

    return roles;
}

Qt::ItemFlags SofaDisplayFlagsTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

bool SofaDisplayFlagsTreeModel::flagEnabled(int flag)
{
    return myFlags & flag;
}

void SofaDisplayFlagsTreeModel::enableFlag(int flag)
{
    myFlags |= flag;
}

void SofaDisplayFlagsTreeModel::disableFlag(int flag)
{
    myFlags &= ~flag;
}

}

}

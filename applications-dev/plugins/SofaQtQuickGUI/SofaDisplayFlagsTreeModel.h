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

#ifndef SOFADISPLAYFLAGSTREEMODEL_H
#define SOFADISPLAYFLAGSTREEMODEL_H

#include "SofaQtQuickGUI.h"
#include "SofaScene.h"
#include "SofaSceneListModel.h"

#include <QAbstractItemModel>

class QTimer;
class QVector3D;

namespace sofa
{

namespace qtquick
{

/// \class A Model allowing us to show a tree of sofa display flags in a TreeView
class SofaDisplayFlagsTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    SofaDisplayFlagsTreeModel(QObject* parent = 0);
    ~SofaDisplayFlagsTreeModel();

    Q_PROPERTY(sofa::qtquick::SofaData* displayFlagsData READ displayFlagsData WRITE setDisplayFlagsData NOTIFY displayFlagsDataChanged)

    Q_INVOKABLE int state(const QModelIndex &index);
    Q_INVOKABLE void setEnabled(const QModelIndex &index);
    Q_INVOKABLE void setDisabled(const QModelIndex &index);

public slots:
    void download();
    void upload();

public:
    SofaData* displayFlagsData() const;
    void setDisplayFlagsData(SofaData* newDisplayFlagsData);

signals:
    void displayFlagsDataChanged(SofaData*);

protected:
    void setupTree();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

private:
    friend class TreeItem;

    bool flagEnabled(int flag);
    void enableFlag(int flag);
    void disableFlag(int flag);

    class TreeItem {
    protected:
        TreeItem(TreeItem* parent, const QString& name, SofaDisplayFlagsTreeModel* tree = nullptr, int flag = -1) :
            myParent(parent),
            myChildren(),
            myName(name),
            myState(Disabled),
            myTree(tree),
            myFlag(flag)
        {

        }

    public:
        ~TreeItem()
        {
            if(myParent)
                myParent->removeChild(this);

            for(TreeItem* child : myChildren)
                delete child;
        }

        static TreeItem* createRoot(const QString& name)
        {
            return new TreeItem(nullptr, name);
        }

        TreeItem* createChild(const QString& name)
        {
            TreeItem* child = new TreeItem(this, name);

            myChildren.append(child);

            return child;
        }

        TreeItem* createChild(const QString& name, SofaDisplayFlagsTreeModel* tree, int flag)
        {
            TreeItem* child = new TreeItem(this, name, tree, flag);

            myChildren.append(child);

            return child;
        }

        void removeChild(TreeItem* child)
        {
            int index = myChildren.indexOf(child);
            if(-1 != index)
                myChildren.takeAt(index);
        }

        TreeItem* parent() const
        {
            return myParent;
        }

        int row() const
        {
            if(myParent)
                return myParent->myChildren.indexOf(const_cast<TreeItem*>(this));

            return 0;
        }

        TreeItem* child(int i) const
        {
            return myChildren.at(i);
        }

        int childCount() const
        {
            return myChildren.size();
        }

        const QString& name() const
        {
            return myName;
        }

        int state() const
        {
            return myState;
        }

        bool enabled() const
        {
            return Enabled == myState;
        }

        bool disabled() const
        {
            return Disabled == myState;
        }

        bool partial() const
        {
            return Partial == myState;
        }

        void setEnabled()
        {
            doSetEnabled();

            if(myParent)
                myParent->checkState();
        }

        void setDisabled()
        {
            doSetDisabled();

            if(myParent)
                myParent->checkState();
        }

        void download() {
            for(TreeItem* child : myChildren)
                child->download();

            if(myTree && -1 != myFlag)
            {
                if(myTree->flagEnabled(myFlag) && Enabled != myState)
                {
                    myState = Enabled;

                    if(myParent)
                        myParent->checkState();
                }
            }
        }

    private:
        void checkState()
        {
            bool isPartial = false;

            bool hasEnabledChild = false;
            bool hasDisabledChild = false;
            bool hasPartialChild = false;

            for(TreeItem* child : myChildren)
            {
                if(child->disabled())
                    hasDisabledChild = true;
                else if(child->enabled())
                    hasEnabledChild = true;
                else
                    hasPartialChild = true;

                isPartial = hasPartialChild || (hasEnabledChild && hasDisabledChild);
                if(isPartial)
                    break;
            }

            int previousState = myState;

            if(isPartial)
                myState = Partial;
            else if(hasEnabledChild)
                myState = Enabled;
            else if(hasDisabledChild)
                myState = Disabled;

            if(previousState != myState && myParent)
                myParent->checkState();
        }

        void doSetEnabled()
        {
            for(TreeItem* child : myChildren)
                child->doSetEnabled();

            myState = Enabled;

            if(myTree && -1 != myFlag)
                myTree->enableFlag(myFlag);
        }

        void doSetDisabled()
        {
            for(TreeItem* child : myChildren)
                child->doSetDisabled();

            myState = Disabled;

            if(myTree && -1 != myFlag)
                myTree->disableFlag(myFlag);
        }

    private:
        TreeItem*                   myParent;
        QList<TreeItem*>            myChildren;
        QString                     myName;
        int                         myState;
        SofaDisplayFlagsTreeModel*  myTree;
        int                         myFlag;

        enum State {
            Disabled = 0,
            Enabled,
            Partial
        };

    };

    enum {
        NameRole = Qt::UserRole + 1
    };

    enum
    {
        HideAll                         = 0,
        ShowVisualModels                = 1,
        ShowBehaviorModels              = 1 << 1,
        ShowForceFields                 = 1 << 2,
        ShowInteractionForceFields      = 1 << 3,
        ShowCollisionModels             = 1 << 4,
        ShowBoundingCollisionsModels    = 1 << 5,
        ShowMappings                    = 1 << 6,
        ShowMechanicalMappings          = 1 << 7,
        ShowAdvancedRendering           = 1 << 8,
        ShowWireframe                   = 1 << 9,
        ShowNormals                     = 1 << 10
    };

private:
    SofaData*       myDisplayFlagsData;
    int             myFlags;

    TreeItem*       myRootItem;

};

}

}

#endif // SOFADISPLAYFLAGSTREEMODEL_H

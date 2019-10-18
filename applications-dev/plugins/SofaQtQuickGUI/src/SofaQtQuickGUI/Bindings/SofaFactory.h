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
#pragma once

#include <SofaQtQuickGUI/config.h>
#include <QAbstractListModel>
#include <QStringList>
#include "SofaNode.h"

namespace sofaqtquick::bindings
{
namespace _sofafactory_
{
using sofaqtquick::bindings::SofaNode;
using sofaqtquick::bindings::SofaBaseObject;

/// The SofaFactory object is in charge of exposing to QML
/// the functionallies of the ObjectFactory including the
/// list of object that can be created.
class SOFA_SOFAQTQUICKGUI_API SofaFactory : public QObject
{
    Q_OBJECT

public:
    SofaFactory(QObject *parent = nullptr);
    ~SofaFactory() override;

    Q_PROPERTY(QStringList components READ getComponents NOTIFY listChanged)

    Q_INVOKABLE bool contains(const QString& name) ;
    Q_INVOKABLE QStringList getComponents() ;
    Q_INVOKABLE QString getComponentHelp(const QString& name) ;
    Q_INVOKABLE void setFilter(const QString& c) ;

    Q_INVOKABLE sofaqtquick::bindings::SofaNode* createNode(const QString name="unnamed") const;

signals:
    void listChanged();

private:
    QString m_filter ;
    QStringList m_filteredList ;
};

} /// _sofafactory_

using _sofafactory_::SofaFactory ;

} /// sofaqtquick::bindings

/*
Copyright 2018,
Author: damien.marchal@univ-lille1.fr, Copyright 2018 CNRS.

This file is part of SofaQtQuick

Sofa is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Sofa. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SOFAQTQUICKSOFA_SOFAFACTORY_H
#define SOFAQTQUICKSOFA_SOFAFACTORY_H

#include "SofaQtQuickGUI.h"
#include <QAbstractListModel>
#include <QStringList>

namespace sofa
{
namespace qtquick
{
namespace _sofafactory_
{

/// The SofaFactory object is in charge of exposing to QML
/// the functionallies of the ObjectFactory including the
/// list of object that can be created.
class SOFA_SOFAQTQUICKGUI_API SofaFactory : public QObject
{
    Q_OBJECT

public:
    SofaFactory(QObject *parent = 0);
    ~SofaFactory();

    Q_PROPERTY(QStringList components READ getComponents NOTIFY listChanged)

    Q_INVOKABLE QStringList getComponents() ;
    Q_INVOKABLE void setFilter(const QString& c) ;

signals:
    void listChanged();

private:
    QString m_filter ;
    QStringList m_filteredList ;
};

} /// _sofafactory_

    using _sofafactory_::SofaFactory ;

} /// qtquick
} /// sofa

#endif /// SOFAQTQUICKSOFA_SOFAFACTORY_H

/*
Copyright 2016,
Author: damien.marchal@univ-lille1.fr, Copyright 2016 CNRS.

This file is part of Sofa

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
#ifndef SOFAFACTORY_H
#define SOFAFACTORY_H

#include <QStringList>
#include <QAbstractItemModel>

#include "SofaQtQuickGUI.h"

namespace sofa
{

namespace qtquick
{

namespace sofafactory
{

/*******************************************************************************
 *  \class SofaFactory expose the Factory API to QT/QML.
 ******************************************************************************/
class SOFA_SOFAQTQUICKGUI_API SofaFactory : public QAbstractItemModel
{
public:
    Q_OBJECT

    /// http://doc.qt.io/qt-5/qobject.html#Q_PROPERTY
    Q_PROPERTY(QStringList categories READ categories) ;

public:
    explicit SofaFactory(QObject *parent = nullptr) ;
    virtual ~SofaFactory() ;

    QStringList categories() const ;

private:
    QStringList m_categories ;
};

}

using sofafactory::SofaFactory ;

}

}

#endif // SOFAFACTORY_H

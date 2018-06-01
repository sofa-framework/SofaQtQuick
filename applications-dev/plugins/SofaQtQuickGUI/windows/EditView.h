/*
Copyright 2018, INRIA

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

#ifndef SOFAQTQUICK_EDITVIEW_H
#define SOFAQTQUICK_EDITVIEW_H

#include "SofaQtQuickGUI.h"
#include "SofaViewer.h"

#include <QtQuick/QQuickFramebufferObject>
#include <QVector3D>
#include <QVector4D>
#include <QImage>
#include <QColor>
#include <QList>

#include <sofa/helper/io/Image.h>

namespace sofa
{
namespace qtquick
{

/// \class Display a Sofa Scene in a QQuickFramebufferObject. The scene elements 
/// can be manipulated by the user through a set of dedicated tools. 
class SOFA_SOFAQTQUICKGUI_API EditView : public SofaViewer
{
    Q_OBJECT

public:
    typedef SofaViewer Inherit;

    explicit EditView(QQuickItem* parent = 0);
    ~EditView();
};

}
}

#endif // SOFAQTQUICK_EDITVIEW_H

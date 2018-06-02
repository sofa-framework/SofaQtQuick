/*
Copyright 2018, CNRS,INRIQ

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

#include <QQuickItem>
#include "CameraView.h"
#include "SofaScene.h"

namespace sofa
{

namespace qtquick
{

CameraView::CameraView(QQuickItem* parent) : Parent(parent)
{

}

CameraView::~CameraView()
{

}

void CameraView::internalRender(int width, int height) const
{
    QSize size(width, height);
    if(size.isEmpty())
        return;

    if(!myCamera)
        return;

    if(mySofaScene && mySofaScene->isReady())
    {
        mySofaScene->clearBuffers(size, QColor(125,125,255,255)) ;
        mySofaScene->setupCamera(width, height, *this) ;
        mySofaScene->setupVisualParams() ;
        preDraw();
        mySofaScene->drawVisuals(*this);
        postDraw();
    }
}

}
}

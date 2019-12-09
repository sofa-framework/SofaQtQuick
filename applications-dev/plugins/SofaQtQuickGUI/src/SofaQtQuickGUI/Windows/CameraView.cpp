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
#include <sofa/core/visual/VisualParams.h>

#include <SofaBaseVisual/BackgroundSetting.h>
using sofa::component::configurationsetting::BackgroundSetting ;

#include <SofaBaseVisual/BaseCamera.h>
using sofa::component::visualmodel::BaseCamera;

#include <SofaQtQuickGUI/Windows/CameraView.h>
#include <SofaQtQuickGUI/Bindings/SofaCamera.h>
#include <SofaQtQuickGUI/SofaBaseScene.h>

namespace sofaqtquick
{

CameraView::CameraView(QQuickItem* parent) : Parent(parent)
{
}

CameraView::~CameraView()
{

}

QColor toQ(const sofa::helper::types::RGBAColor& color)
{
    return QColor(color.r()*255, color.g()*255, color.b()*255, color.a()*255) ;
}

void CameraView::internalRender(int width, int height) const
{
    if(!myCamera){
        QColor color(128,128,244,255);
        QSize size(width, height);
        clearBuffers(size, color);
        return ;
    }

    QSize size(width, height);
    if(size.isEmpty())
        return;

    SofaCamera* sofaCamera = dynamic_cast<SofaCamera*>(myCamera) ;
    if(!sofaCamera){
        QColor color(128,128,244,255);
        QSize size(width, height);
        clearBuffers(size, color);

        return;
    }

    BaseCamera* baseCamera = sofaCamera->getBaseCamera();
    if(!baseCamera)
    {
        QColor color(128,128,244,255);
        QSize size(width, height);
        clearBuffers(size, color);
        return;
    }
    if(mySofaScene && mySofaScene->isReady())
    {
        NewViewerContext pushpop;

        BackgroundSetting* settings = sofaCamera->getBaseCamera()->l_background.get();
        QColor color(128,128,244,255);
        QImage image;

        if(settings){
            if(settings->color.isSet())
                color=toQ( settings->color.getValue());

            if(settings->image.isSet())
            {
                clearBuffers(size, color);
                msg_error("runSofa2::cameraview does not support yet background pictures.");
            }
        }
        clearBuffers(size, color, image);
        setupCamera(width, height, *this) ;

        /// Prepare for a pure rendrering traversal of the scene graph.
        auto oldFlags {m_visualParams->displayFlags()};
        m_visualParams->displayFlags().setShowAll(false) ;
        m_visualParams->displayFlags().setShowVisualModels(true) ;

        preDraw();
        drawVisuals();
        postDraw();

        m_visualParams->displayFlags() = oldFlags;
    }
}

}

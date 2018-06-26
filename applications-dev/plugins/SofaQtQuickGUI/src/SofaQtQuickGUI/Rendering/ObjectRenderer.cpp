#include <sofa/core/objectmodel/Base.h>
using sofa::core::objectmodel::Base;

#include <sofa/core/visual/VisualModel.h>
using sofa::core::visual::VisualModel;

#include <sofa/core/visual/VisualParams.h>
using sofa::core::visual::VisualParams ;

#include <SofaBaseVisual/VisualStyle.h>
using sofa::component::visualmodel::VisualStyle;

#include <SofaBaseVisual/VisualStyle.h>
using sofa::component::visualmodel::VisualStyle;

#include <SofaMeshCollision/TriangleModel.h>
using sofa::component::collision::TriangleModel ;
using sofa::core::CollisionModel ;

#include <SofaOpenglVisual/Light.h>
using sofa::component::visualmodel::Light ;

#include <SofaBaseVisual/BaseCamera.h>
using sofa::component::visualmodel::BaseCamera ;

#include "ObjectRenderer.h"

namespace sofa
{
namespace qtquick
{
namespace _objectrenderer_
{

void drawCamera(BaseCamera* camera, VisualParams* VisualParams, bool isSelected)
{
}

void drawLight(Light* light, VisualParams* visualParams, bool isSelected)
{
    light->drawSource(visualParams) ;
}

void drawVisualModel(VisualModel* visualModel, VisualParams* visualParams, bool isSelected)
{
    BaseCamera* camera = dynamic_cast<BaseCamera*>(visualModel) ;
    if(camera)
        return drawCamera(camera, visualParams, isSelected);

    Light* light = dynamic_cast<Light*>(visualModel) ;
    if(light)
        return drawLight(light, visualParams, isSelected);

    VisualStyle* visualStyle = nullptr;
    visualModel->getContext()->get(visualStyle);

    if(visualStyle)
        visualStyle->fwdDraw(visualParams);

    sofa::core::visual::tristate state = visualParams->displayFlags().getShowWireFrame();
    visualParams->displayFlags().setShowWireFrame(true);
    visualModel->drawVisual(visualParams);
    visualParams->displayFlags().setShowWireFrame(state);

    if(visualStyle)
        visualStyle->bwdDraw(visualParams);
}

void drawCollisionModel(CollisionModel* object, VisualParams *visualParams, bool isSelected)
{
    TriangleModel* triangleModel = dynamic_cast<TriangleModel*>(object);
    if(!triangleModel)
        return ;

    VisualStyle* visualStyle = nullptr;
    triangleModel->getContext()->get(visualStyle);

    if(visualStyle)
        visualStyle->fwdDraw(visualParams);

    sofa::core::visual::tristate state = visualParams->displayFlags().getShowWireFrame();
    visualParams->displayFlags().setShowWireFrame(true);

    triangleModel->draw(visualParams);

    visualParams->displayFlags().setShowWireFrame(state);

    if(visualStyle)
        visualStyle->bwdDraw(visualParams);
}

void ObjectRenderer::drawBaseObject(Base* object, VisualParams* visualParams, bool isSelected)
{
    VisualModel* visualModel = object->toVisualModel();
    if(visualModel)
        drawVisualModel(visualModel, visualParams, isSelected) ;

    CollisionModel* collisionModel = object->toCollisionModel() ;
    if(collisionModel)
        drawCollisionModel(collisionModel, visualParams, isSelected) ;
}


} ///
} /// qtquick
} /// sofa

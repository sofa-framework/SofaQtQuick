#include <sofa/core/objectmodel/Base.h>
using sofa::core::objectmodel::Base;

#include <sofa/core/objectmodel/BaseNode.h>
using sofa::core::objectmodel::BaseNode;

#include <sofa/core/objectmodel/BaseObject.h>
using sofa::core::objectmodel::BaseObject;

#include <sofa/simulation/Node.h>
using sofa::simulation::Node;

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

namespace sofaqtquick
{
namespace _objectrenderer_
{

void drawCamera(BaseCamera* camera, VisualParams* visualParams, bool isSelected)
{
    camera->drawCamera(visualParams);
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

    auto oldState{ visualParams->displayFlags() };
    visualParams->displayFlags().setShowWireFrame(true);
    visualModel->drawVisual(visualParams);
    visualParams->displayFlags() = oldState;

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

    auto oldState{ visualParams->displayFlags() };
    visualParams->displayFlags().setShowWireFrame(true);

    triangleModel->draw(visualParams);

    visualParams->displayFlags()=oldState;

    if(visualStyle)
        visualStyle->bwdDraw(visualParams);
}



void drawBaseObject(BaseObject* object, Base* selected, VisualParams* visualParams, bool isSelected)
{
    if(object==nullptr)
        return;

    if(selected != object)
        return;

    BaseCamera* camera = dynamic_cast<BaseCamera*>(object) ;
    if(camera)
        return drawCamera(camera, visualParams, isSelected);

    VisualModel* visualModel = object->toVisualModel();
    if(visualModel)
        drawVisualModel(visualModel, visualParams, isSelected) ;

    CollisionModel* collisionModel = object->toCollisionModel() ;
    if(collisionModel)
        drawCollisionModel(collisionModel, visualParams, isSelected) ;    


    auto oldState{ visualParams->displayFlags() };
    visualParams->displayFlags().setShowAll(true);

    object->draw(visualParams);

    visualParams->displayFlags()=oldState;
}

void drawBaseNode(BaseNode* node, Base* selected, VisualParams* visualParams, bool isSelected)
{
    /// Iterates over the childs.
    for(auto& child : node->getChildren())
        drawBaseNode(child, selected, visualParams, isSelected);

    Node* nnode = dynamic_cast<Node*>(node);
    if(nnode)
    {
        /// Iterates over the components to render them.
        for(auto& object : nnode->getNodeObjects())
            drawBaseObject(object, selected, visualParams, isSelected);
    }
}

void ObjectRenderer::draw(Base* object, Base* selected, VisualParams* visualParams, bool isSelected)
{
    if(object==nullptr)
        return;

    if(object->toBaseNode())
        return drawBaseNode(object->toBaseNode(), object, visualParams, isSelected);

    if(object->toBaseObject())
        return drawBaseObject(object->toBaseObject(), object, visualParams, isSelected);

}

} ///
} /// sofaqtquick

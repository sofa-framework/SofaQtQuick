#pragma once

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/objectmodel/DataFileName.h>
#include <sofa/core/DataTracker.h>
#include <QmlUI/initQmlUI.h>

namespace qmlui
{
  /**
     Canvas is a Scene graph component that lets you customize the CameraView
     of runSofa2 by overlaying a QML-written canvas on top of it.
     By inheriting BaseObject, the canvas becomes a scene graph element,
     and can expose data fields, request the component's context etc.
  **/
  class SOFA_QMLUI_API Canvas : public sofa::core::objectmodel::BaseObject
  {
  public:
    SOFA_CLASS(Canvas, sofa::core::objectmodel::BaseObject);

    Canvas();
    void init() override;

    sofa::core::objectmodel::DataFileName d_qmlFile; //> The QML file describing the UI
    sofa::Data<int> d_x; //> The X margin for this Canvas
    sofa::Data<int> d_y; //> The Y margin for this Canvas

    sofa::Data<uint> d_lastModified;
    sofa::core::DataTracker tracker;
  };
  
}  // namespace qmlui

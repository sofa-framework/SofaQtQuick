#include "Canvas.h"
#include <sofa/core/ObjectFactory.h>
#include <SofaBaseVisual/BaseCamera.h>

namespace qmlui
{

  Canvas::Canvas()
    : d_qmlFile(initData(&d_qmlFile, "filename", "The QML ui file to load as canvas")),
      d_x(initData(&d_x, 0, "x", "the X coordinate of the canvas")),
      d_y(initData(&d_y, 0, "y", "the Y coordinate of the canvas")),
      d_lastModified(initData(&d_lastModified, uint(0), "lastModified",
                              "the Timestamp of the last modification of filename", false, true))
  {
      m_componentstate = sofa::core::objectmodel::ComponentState::Loading;
      d_qmlFile.setRequired(true);
  }

  void Canvas::init()
  {
      tracker.trackData(d_qmlFile);
      tracker.trackData(d_lastModified);
      if (!d_qmlFile.isSet() || d_qmlFile.getExtension() != "qml")
      {
          m_componentstate = sofa::core::objectmodel::ComponentState::Invalid;
          msg_error("Canvas") << "Canvas initialized without a qml UI file";
      }
      else
      {
          m_componentstate = sofa::core::objectmodel::ComponentState::Valid;
      }
  }
  
  static int CanvasClass = sofa::core::RegisterObject("A QML Canvas for the CameraView")
          .add< Canvas >(true);

}  // namespace qmlui


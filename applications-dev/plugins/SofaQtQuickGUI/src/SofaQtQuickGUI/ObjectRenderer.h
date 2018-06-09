#ifndef SOFAQTQUICKGUI_OBJECTRENDERER_H
#define SOFAQTQUICKGUI_OBJECTRENDERER_H

namespace sofa {
    namespace core{
        namespace objectmodel {
            class Base ;
        }
        namespace visual{
            class VisualParams;
        }
    }
}

namespace sofa
{
namespace qtquick
{
namespace _objectrenderer_
{

using sofa::core::objectmodel::Base ;
using sofa::core::visual::VisualParams ;

class ObjectRenderer
{
public:
    static void drawBaseObject(Base*, VisualParams* params, bool isSelected);
};

} /// objectrenderer_h

using _objectrenderer_::ObjectRenderer ;

} /// qtquick
} /// sofa
#endif /// SOFAQTQUICKGUI_OBJECTRENDERER_H

#ifndef SOFAQTQUICKGUI_OBJECTRENDERER_H
#define SOFAQTQUICKGUI_OBJECTRENDERER_H

//////////////////////////////// Forward declaration ///////////////////////////////////////////////
namespace sofa {
    namespace core{
        namespace objectmodel {
            class Base ;
        }
        namespace visual{
            class VisualParams;
        }
    } /// namespace core
} /// namespace sofa


//////////////////////////////// Definition ////////////////////////////////////////////////////////
namespace sofaqtquick
{
namespace _objectrenderer_
{

using sofa::core::objectmodel::Base ;
using sofa::core::visual::VisualParams ;

///
/// \brief The ObjectRenderer class in charge of displaying the objects in the editor views
///
/// General design:
///  - Each editor's view has its own set of display flags
///  - The display flags are used to control the rendering of objects
///  - When a component/object is selected, the view is altered to represent
///    component specific information. This dedicated rendering is done
///    on in ObjectRenderer
///
class ObjectRenderer
{
public:
    static void draw(Base* object, Base* selected, VisualParams* params, bool isSelected);
};

} /// objectrenderer_h

using _objectrenderer_::ObjectRenderer ;

} /// sofaqtquick

#endif /// SOFAQTQUICKGUI_OBJECTRENDERER_H

#ifndef SOFAQTQUICKGUI_SCENERENDERER_H
#define SOFAQTQUICKGUI_SCENERENDERER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

//////////////////////////////// Forward declaration ///////////////////////////////////////////////


//////////////////////////////// Definition ////////////////////////////////////////////////////////
namespace sofa
{
namespace qtquick
{
namespace _scenerenderer_
{

class SceneRenderer
{
public:
    QOpenGLShaderProgram*                       myHighlightShaderProgram {nullptr};
    QOpenGLShaderProgram*                       myPickingShaderProgram {nullptr};
};

}

using _scenerenderer_::SceneRenderer ;

} /// qtquick
} /// sofa
#endif /// SOFAQTQUICKGUI_SCENERENDERER_H

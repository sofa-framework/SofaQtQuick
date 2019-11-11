#ifndef SOFAQTQUICKGUI_SCENERENDERER_H
#define SOFAQTQUICKGUI_SCENERENDERER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

//////////////////////////////// Forward declaration ///////////////////////////////////////////////


//////////////////////////////// Definition ////////////////////////////////////////////////////////
namespace sofaqtquick
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

} /// sofaqtquick

#endif /// SOFAQTQUICKGUI_SCENERENDERER_H

/*
Copyright 2015, Anatoscope
Copyright 2016, CNRS

Contributors:
    damien.marchal@univ-lille1.fr

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

#include <SofaQtQuickGUI/RuntimeViewer.h>
#include <SofaQtQuickGUI/SofaScene.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator.h>

#include <sofa/simulation/Node.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <SofaOpenglVisual/OglModel.h>
#include <sofa/helper/cast.h>

#include <QtQuick/qquickwindow.h>
#include <QQmlEngine>
#include <QQmlContext>
#include <QRunnable>
#include <QEventLoop>
//#include <QOpenGLContext>
//#include <QOpenGLPaintDevice>
//#include <QPaintEngine>
//#include <QPainter>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QSGTransformNode>
#include <QSGSimpleTextureNode>
#include <QVector>
#include <QVector4D>
#include <QTime>
#include <QPair>
#include <QThread>
#include <QString>
#include <QDir>
#include <qqml.h>
#include <qmath.h>
#include <limits>

#include <iomanip>
#include <sstream>

using sofa::simulation::Node;
using sofa::component::visualmodel::OglModel;



namespace sofa
{

namespace qtquick
{

using namespace sofa::simulation;

RuntimeViewer::RuntimeViewer(QQuickItem* parent) : QQuickFramebufferObject(parent),
    myFBO(nullptr),
    mySofaScene(nullptr),
    myCamera(nullptr),
    myRoots(),
    myBackgroundColor("#00404040"),
    myAntialiasingSamples(2)
{
    setMirrorVertically(true);

    setFlag(QQuickItem::ItemHasContents);

    connect(this, SIGNAL(antialiasingSamplesChanged(int)), this, SLOT(update()));
}

RuntimeViewer::~RuntimeViewer()
{
    /*sofa::core::visual::VisualParams* _vparams = sofa::core::visual::VisualParams::defaultInstance();
    if(_vparams && _vparams->drawTool())
    {
        delete _vparams->drawTool();
        _vparams->drawTool() = 0;
    }*/

    clearRoots();
}

void RuntimeViewer::setSofaScene(SofaScene* newSofaScene)
{
    if(newSofaScene == mySofaScene)
        return;

    mySofaScene = newSofaScene;

    sofaSceneChanged(newSofaScene);
}

void RuntimeViewer::setCamera(Camera* newCamera)
{
    if(newCamera == myCamera)
        return;

    myCamera = newCamera;

    cameraChanged(newCamera);
}

static void appendRoot(QQmlListProperty<SofaComponent> *property, SofaComponent *value)
{
    static_cast<QList<SofaComponent*>*>(property->data)->append(value ? new SofaComponent(*value) : nullptr);
}

static SofaComponent* atRoot(QQmlListProperty<SofaComponent> *property, int index)
{
    return static_cast<QList<SofaComponent*>*>(property->data)->at(index);
}

static void clearRoot(QQmlListProperty<SofaComponent> *property)
{
    QList<SofaComponent*>& roots = *static_cast<QList<SofaComponent*>*>(property->data);

    for(SofaComponent* sofaComponent : roots)
        delete sofaComponent;

    roots.clear();
}

static int countRoot(QQmlListProperty<SofaComponent> *property)
{
    return static_cast<QList<SofaComponent*>*>(property->data)->size();
}

QList<SofaComponent*> RuntimeViewer::roots() const
{
    return myRoots;
}

QQmlListProperty<SofaComponent> RuntimeViewer::rootsListProperty()
{
    return QQmlListProperty<SofaComponent>(this, &myRoots, appendRoot, countRoot, atRoot, clearRoot);
}

void RuntimeViewer::clearRoots()
{
    myRoots.clear();
}

void RuntimeViewer::setBackgroundColor(QColor newBackgroundColor)
{
    if(newBackgroundColor == myBackgroundColor)
        return;

    myBackgroundColor = newBackgroundColor;

    backgroundColorChanged(newBackgroundColor);
}

void RuntimeViewer::setAntialiasingSamples(int newAntialiasingSamples)
{
    if(newAntialiasingSamples == myAntialiasingSamples)
        return;

    myAntialiasingSamples = newAntialiasingSamples;

    antialiasingSamplesChanged(newAntialiasingSamples);
}


QPair<QVector3D, QVector3D> RuntimeViewer::boundingBox() const
{
    QVector3D min, max;
    mySofaScene->computeBoundingBox(min, max);

    return QPair<QVector3D, QVector3D>(min, max);
}

QVector3D RuntimeViewer::boundingBoxMin() const
{
    QVector3D min, max;
    mySofaScene->computeBoundingBox(min, max);

    return min;
}

QVector3D RuntimeViewer::boundingBoxMax() const
{
    QVector3D min, max;
    mySofaScene->computeBoundingBox(min, max);

    return max;
}


void RuntimeViewer::saveScreenshot(const QString& path)
{
    QDir dir = QFileInfo(path).dir();
    if(!dir.exists())
        dir.mkpath(".");

    if(!myFBO->toImage().save(path))
        msg_error("SofaQtQuickGUI") << "Screenshot could not be saved to" << path.toStdString();
}

class ScreenshotWorker : public QRunnable
{
public:
    ScreenshotWorker(const RuntimeViewer* viewer, const QString& path, int width, int height) :
        myViewer(viewer),
        myPath(path),
        myWidth(width),
        myHeight(height)
    {

    }

    void run()
    {
        QSize size(myWidth, myHeight);
        if(size.isEmpty())
            return;

        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(myViewer->antialiasingSamples());

        glViewport(0.0, 0.0, myWidth, myHeight);

        QOpenGLFramebufferObject fbo(myWidth, myHeight, format);
        fbo.bind();

        myViewer->internalRender(myWidth, myHeight);

        fbo.release();

        if(!fbo.toImage().save(myPath))
            msg_error("SofaQtQuickGUI") << "Screenshot could not be saved to" << myPath.toStdString();
    }

private:
    const RuntimeViewer*	myViewer;
    QString				myPath;
    int					myWidth;
    int					myHeight;

};

void RuntimeViewer::saveScreenshotWithResolution(const QString& path, int width, int height)
{
    if(!window())
        return;

    QDir dir = QFileInfo(path).dir();
    if(!dir.exists())
        dir.mkpath(".");

    ScreenshotWorker* worker = new ScreenshotWorker(this, path, width, height);
    window()->scheduleRenderJob(worker, QQuickWindow::AfterSynchronizingStage);
    window()->update();
}

QSize RuntimeViewer::nativeSize() const
{
    return nativeRect().size();
}

QRect RuntimeViewer::nativeRect() const
{
    if(!window())
        return QRect();

    QPointF realPos = mapToScene(QPointF(0.0, qCeil(height())));
    realPos.setX(realPos.x() * window()->devicePixelRatio());
    realPos.setY((window()->height() - realPos.y()) * window()->devicePixelRatio());  // OpenGL has its Y coordinate inverted compared to Qt

    QPoint pos(qFloor(realPos.x()), qFloor(realPos.y()));
    QSize size((qCeil(width()) + qCeil(pos.x() - realPos.x())) * window()->devicePixelRatio(), (qCeil((height()) + qCeil(pos.y() - realPos.y())) * window()->devicePixelRatio()));

    return QRect(pos, size);
}

QRect RuntimeViewer::qtRect() const
{
    if(!window())
        return QRect();

    QPointF realPos = mapToScene(QPointF(0.0, qCeil(height())));
    realPos.setX( realPos.x() * window()->devicePixelRatio());
    realPos.setY( realPos.y() * window()->devicePixelRatio());

    QPoint pos(qFloor(realPos.x()), qFloor(realPos.y()));
    QSize size((qCeil(width()) + qCeil(pos.x() - realPos.x())) * window()->devicePixelRatio(), (qCeil((height()) + qCeil(pos.y() - realPos.y())) * window()->devicePixelRatio()));

    return QRect(pos, size);
}

QPointF RuntimeViewer::mapToNative(const QPointF& ssPoint) const
{
    QPointF ssNativePoint(ssPoint);

    ssNativePoint.setX(ssNativePoint.x() * window()->devicePixelRatio());
    ssNativePoint.setY(ssNativePoint.y() * window()->devicePixelRatio());

    return ssNativePoint;
}

void RuntimeViewer::viewAll(float radiusFactor)
{
    if(!myCamera || !mySofaScene || !mySofaScene->isReady())
        return;

    QVector3D min, max;
    mySofaScene->computeBoundingBox(min, max, myRoots);

    myCamera->fit(min, max, 1.5f * radiusFactor);
}

QSGNode* RuntimeViewer::updatePaintNode(QSGNode* inOutNode, UpdatePaintNodeData* inOutData)
{
    if(!inOutNode)
    {
        inOutNode = QQuickFramebufferObject::updatePaintNode(inOutNode, inOutData);
        return inOutNode;
    }
    return QQuickFramebufferObject::updatePaintNode(inOutNode, inOutData);
}

void RuntimeViewer::internalRender(int width, int height) const
{
    QSize size(width, height);
    if(size.isEmpty())
        return;

    // final image will be blended using premultiplied alpha
    glClearColor(myBackgroundColor.redF() * myBackgroundColor.alphaF(),
                 myBackgroundColor.greenF() * myBackgroundColor.alphaF(),
                 myBackgroundColor.blueF() * myBackgroundColor.alphaF(),
                 myBackgroundColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!myCamera)
        return;

    // set default lights
    {
        glEnable(GL_LIGHT0);
        {
            float lightPosition[] = { 0.5f,  0.5f, 1.0f, 0.0f};
            float lightAmbient [] = { 0.0f,  0.0f, 0.0f, 0.0f};
            float lightDiffuse [] = { 1.0f,  1.0f, 1.0f, 1.0f};
            float lightSpecular[] = { 0.0f,  0.0f, 0.0f, 0.0f};

            glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
            glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
            glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
        }

        glEnable(GL_LIGHT1);
        {
            float lightPosition[] = { -1.0f, -1.0f,-1.0f, 0.0f};
            float lightAmbient [] = {  0.0f,  0.0f, 0.0f, 0.0f};
            float lightDiffuse [] = { 0.25f, 0.25f, 0.5f, 0.0f};
            float lightSpecular[] = {  0.0f,  0.0f, 0.0f, 0.0f};

            glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
            glLightfv(GL_LIGHT1, GL_AMBIENT,  lightAmbient);
            glLightfv(GL_LIGHT1, GL_DIFFUSE,  lightDiffuse);
            glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
        }
    }

    glEnable(GL_LIGHTING);

    if(mySofaScene && mySofaScene->isReady())
    {
        myCamera->setAspectRatio(width / (double) height);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(myCamera->projection().constData());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(myCamera->view().constData());

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);

        // qt does not release its shader program and we do not use one so we have to release the current bound program
        glUseProgram(0);

        // prepare the sofa visual params
        sofa::core::visual::VisualParams* _vparams = sofa::core::visual::VisualParams::defaultInstance();
        if(_vparams)
        {
            if(!_vparams->drawTool())
            {
                _vparams->drawTool() = new sofa::core::visual::DrawToolGL();
                _vparams->setSupported(sofa::core::visual::API_OpenGL);
            }

            GLint _viewport[4];
            GLdouble _mvmatrix[16], _projmatrix[16];


            glGetIntegerv (GL_VIEWPORT, _viewport);
            glGetDoublev  (GL_MODELVIEW_MATRIX, _mvmatrix);
            glGetDoublev  (GL_PROJECTION_MATRIX, _projmatrix);

            _vparams->viewport() = sofa::helper::fixed_array<int, 4>(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
            _vparams->sceneBBox() = mySofaScene->sofaRootNode()->f_bbox.getValue();
            _vparams->setProjectionMatrix(_projmatrix);
            _vparams->setModelViewMatrix(_mvmatrix);
        }

        // draw the SofaScene
        {
            preDraw();
            mySofaScene->draw(*this, roots());
            postDraw();
        }

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}

void RuntimeViewer::renderFrame() const
{
    sofa::core::visual::VisualParams* _vparams = sofa::core::visual::VisualParams::defaultInstance();
    if(!_vparams)
        return;

    float size = 1.0f;
    if(mySofaScene)
        size = mySofaScene->radius() * 0.1;

    if(0.0f == size)
        size = 1.0f;

    _vparams->drawTool()->drawFrame(sofa::defaulttype::Vector3(), sofa::defaulttype::Quaternion(), sofa::defaulttype::Vector3(size, size, size));
}

RuntimeViewer::SofaRenderer::SofaRenderer(RuntimeViewer* viewer) : QQuickFramebufferObject::Renderer(),
    myViewer(viewer),
    myAntialiasingSamples(0)
{

}

QOpenGLFramebufferObject* RuntimeViewer::SofaRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(myAntialiasingSamples);

    myViewer->myFBO = new QOpenGLFramebufferObject(size, format);
    return myViewer->myFBO;
}

void RuntimeViewer::SofaRenderer::synchronize(QQuickFramebufferObject* quickFramebufferObject)
{
    RuntimeViewer* viewer = qobject_cast<RuntimeViewer*>(quickFramebufferObject);
    if(!viewer)
        return;

    if(myAntialiasingSamples != viewer->myAntialiasingSamples)
    {
        myAntialiasingSamples = viewer->myAntialiasingSamples;
        invalidateFramebufferObject();
    }
}

void RuntimeViewer::SofaRenderer::render()
{
    update();

    if(!myViewer || !myViewer->isVisible())
        return;

    myViewer->internalRender(myViewer->width(), myViewer->height());
}

}

}

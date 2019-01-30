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

#ifndef RUNTIMEVIEWER_H
#define RUNTIMEVIEWER_H

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
#include <SofaQtQuickGUI/Camera.h>
#include <SofaQtQuickGUI/SelectableSofaParticle.h>

#include <QtQuick/QQuickFramebufferObject>
#include <QVector3D>
#include <QVector4D>
#include <QImage>
#include <QColor>
#include <QList>

#include <sofa/helper/io/Image.h>

namespace sofa
{

namespace qtquick
{

class SofaRenderer;
class SofaComponent;
class SofaScene;
class Camera;
class Manipulator;

class PickUsingRasterizationWorker;
class ScreenshotWorker;

/// \class Display in a QQuickFramebufferObject what is viewed by the current sofa camera.
/// \note Coordinate prefix meaning:
/// ws  => world space
/// vs  => view space
/// cs  => clip space
/// ndc => ndc space
/// ss  => screen space (window space)
class SOFA_SOFAQTQUICKGUI_API RuntimeViewer : public QQuickFramebufferObject
{
    Q_OBJECT


    friend class PickUsingRasterizationWorker;
    friend class ScreenshotWorker;
    friend class SofaRenderer;

public:
    explicit RuntimeViewer(QQuickItem* parent = nullptr);
    ~RuntimeViewer();

public:
    Q_PROPERTY(sofa::qtquick::SofaScene* sofaScene READ sofaScene WRITE setSofaScene NOTIFY sofaSceneChanged)
    Q_PROPERTY(sofa::qtquick::Camera* camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QQmlListProperty<sofa::qtquick::SofaComponent> roots READ rootsListProperty)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(int antialiasingSamples READ antialiasingSamples WRITE setAntialiasingSamples NOTIFY antialiasingSamplesChanged)

public:
    Renderer* createRenderer() const {return new SofaRenderer(const_cast<RuntimeViewer*>(this));}

    SofaScene* sofaScene() const        {return mySofaScene;}
    void setSofaScene(SofaScene* newScene);

    Camera* camera() const      {return myCamera;}
    void setCamera(Camera* newCamera);

    QList<SofaComponent*> roots() const;
    QQmlListProperty<SofaComponent> rootsListProperty();
    void clearRoots();

    QColor backgroundColor() const	{return myBackgroundColor;}
    void setBackgroundColor(QColor newBackgroundColor);

    int antialiasingSamples() const        {return myAntialiasingSamples;}
    void setAntialiasingSamples(int newAntialiasingSamples);

    /// @brief map screen coordinates to opengl coordinates
    QPointF mapToNative(const QPointF& ssPoint) const;

    QSize nativeSize() const;

    Q_INVOKABLE QPair<QVector3D, QVector3D> boundingBox() const;
    Q_INVOKABLE QVector3D boundingBoxMin() const;
    Q_INVOKABLE QVector3D boundingBoxMax() const;

    Q_INVOKABLE void saveScreenshot(const QString& path);
    Q_INVOKABLE void saveScreenshotWithResolution(const QString& path, int width, int height);

signals:
    void sofaSceneChanged(sofa::qtquick::SofaScene* newScene);
    void rootsChanged(QList<sofa::qtquick::SofaComponent> newRoots);
    void cameraChanged(sofa::qtquick::Camera* newCamera);
    void backgroundColorChanged(QColor newBackgroundColor);
    void antialiasingSamplesChanged(int newAntialiasingSamples);

    void preDraw() const;
    void postDraw() const;

public slots:
    void viewAll(float radiusFactor = 1.0f); // radiusFactor scales the bounding box used to compute the zoom level

protected:
    QSGNode* updatePaintNode(QSGNode* inOutNode, UpdatePaintNodeData* inOutData);
    void internalRender(int width, int height) const;
    void renderFrame() const;

private:
    QRect nativeRect() const;
    QRect qtRect() const;

private:
    class SofaRenderer : public QQuickFramebufferObject::Renderer
    {
    public:
        SofaRenderer(RuntimeViewer* viewer);

    protected:
        QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
        void synchronize(QQuickFramebufferObject* quickFramebufferObject);
        void render();

    private:
        // TODO: not safe at all when we will use multithreaded rendering, use synchronize() instead
        RuntimeViewer* myViewer;
        int            myAntialiasingSamples;

    };

private:
    QOpenGLFramebufferObject*   myFBO;
    SofaScene*                  mySofaScene;
    Camera*						myCamera;
    QList<SofaComponent*>       myRoots;
    QColor                      myBackgroundColor;
    int                         myAntialiasingSamples;
};

}

}

#endif // RUNTIMEVIEWER_H

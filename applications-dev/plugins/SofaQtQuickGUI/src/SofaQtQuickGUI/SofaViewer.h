/*
Copyright 2015, Anatoscope

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

#ifndef SOFAVIEWER_H
#define SOFAVIEWER_H

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Camera.h>
#include <SofaQtQuickGUI/SelectableSofaParticle.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator.h>

#include <GL/glew.h>
#include <QOpenGLShaderProgram>
#include <QtQuick/QQuickFramebufferObject>
#include <QVector3D>
#include <QVector4D>
#include <QImage>
#include <QColor>
#include <QList>

#include <sofa/helper/io/Image.h>
namespace sofaqtquick
{

class SofaRenderer;
class SofaBaseScene;
class Camera;
class Manipulator;

class PickUsingRasterizationWorker;
class ScreenshotWorker;


class NewViewerContext
{
public:
    NewViewerContext();
    ~NewViewerContext();
};

/// \class Display a Sofa Scene in a QQuickFramebufferObject
/// \note Coordinate prefix meaning:
/// ws  => world space
/// vs  => view space
/// cs  => clip space
/// ndc => ndc space
/// ss  => screen space (window space)
class SOFA_SOFAQTQUICKGUI_API SofaViewer : public QQuickFramebufferObject
{
    Q_OBJECT

    friend class PickUsingRasterizationWorker;
	friend class ScreenshotWorker;
    friend class SofaRenderer;

public:
    explicit SofaViewer(QQuickItem* parent = nullptr);
    ~SofaViewer();

public:
    Q_PROPERTY(sofaqtquick::SofaBaseScene* sofaScene READ sofaScene WRITE setSofaScene NOTIFY sofaSceneChanged)
    Q_PROPERTY(sofaqtquick::Camera* camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QQmlListProperty<sofaqtquick::bindings::SofaBase> roots READ rootsListProperty)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QUrl backgroundImageSource READ backgroundImageSource WRITE setBackgroundImageSource NOTIFY backgroundImageSourceChanged)
    Q_PROPERTY(int antialiasingSamples READ antialiasingSamples WRITE setAntialiasingSamples NOTIFY antialiasingSamplesChanged)
    Q_PROPERTY(bool mirroredHorizontally READ mirroredHorizontally WRITE setMirroredHorizontally NOTIFY mirroredHorizontallyChanged)
    Q_PROPERTY(bool mirroredVertically READ mirroredVertically WRITE setMirroredVertically NOTIFY mirroredVerticallyChanged)
    Q_PROPERTY(bool drawFrame READ drawFrame WRITE setDrawFrame NOTIFY drawFrameChanged)
    Q_PROPERTY(bool drawManipulators READ drawManipulators WRITE setDrawManipulators NOTIFY drawManipulatorsChanged)
    Q_PROPERTY(bool drawSelected READ drawSelected WRITE setDrawSelected NOTIFY drawSelectedChanged)
    Q_PROPERTY(bool alwaysDraw READ alwaysDraw WRITE setAlwaysDraw NOTIFY alwaysDrawChanged) /// \brief always draw the scene in the fbo
    Q_PROPERTY(bool autoPaint READ autoPaint WRITE setAutoPaint NOTIFY autoPaintChanged) /// \brief paint the fbo on the screen every frame, if false: you must call update() to request a paint
    Q_PROPERTY(QQmlListProperty<sofaqtquick::Manipulator> manipulators READ manipulators)
    Q_PROPERTY(float sceneUnits READ sceneUnits WRITE setSceneUnits NOTIFY sceneUnitsChanged)

public:
    Renderer* createRenderer() const {return new SofaRenderer(const_cast<SofaViewer*>(this));}

    SofaBaseScene* sofaScene() const        {return mySofaScene;}
    void setSofaScene(SofaBaseScene* newScene);

    sofaqtquick::Camera* camera() const      {return myCamera;}
    void setCamera(sofaqtquick::Camera* newCamera);

    QList<sofaqtquick::bindings::SofaBase*> roots() const;
    QQmlListProperty<sofaqtquick::bindings::SofaBase> rootsListProperty();
    void clearRoots();

    QColor backgroundColor() const	{return myBackgroundColor;}
    void setBackgroundColor(QColor newBackgroundColor);

    QUrl backgroundImageSource() const	{return myBackgroundImageSource;}
    void setBackgroundImageSource(QUrl newBackgroundImageSource);

    int antialiasingSamples() const        {return myAntialiasingSamples;}
    void setAntialiasingSamples(int newAntialiasingSamples);

    bool mirroredHorizontally() const        {return myMirroredHorizontally;}
    void setMirroredHorizontally(bool newMirroredHorizontally);

    bool mirroredVertically() const        {return myMirroredVertically;}
    void setMirroredVertically(bool newMirroredVertically);

    bool drawFrame() const {return myDrawFrame;}
    void setDrawFrame(bool newDrawFrame);

    bool drawManipulators() const {return myDrawManipulators;}
    void setDrawManipulators(bool newDrawManipulators);

    bool drawSelected() const {return myDrawSelected;}
    void setDrawSelected(bool newDrawSelected);

	bool alwaysDraw() const { return myAlwaysDraw; }
	void setAlwaysDraw(bool myAlwaysDraw);

    float sceneUnits() const { return mySceneUnits; }
    void setSceneUnits(float units)
    {
        if (mySceneUnits != units)
        {
            mySceneUnits = units;
            emit sceneUnitsChanged(mySceneUnits);
        }
    }

    sofa::core::visual::VisualParams* getVisualParams() const { return m_visualParams; }

    bool autoPaint() const { return myAutoPaint; }
    Q_SLOT void setAutoPaint(bool newAutoPaint) { if(myAutoPaint == newAutoPaint) return; myAutoPaint = newAutoPaint; autoPaintChanged(newAutoPaint); }
    Q_SIGNAL void autoPaintChanged(bool newAutoPaint);

    /// @return depth in screen space
    Q_INVOKABLE double computeDepth(const QVector3D& wsPosition) const;

    Q_INVOKABLE QVector3D mapFromWorld(const QVector3D& wsPoint) const;
    Q_INVOKABLE QVector3D mapToWorld(const QPointF& ssPoint, double z) const;

    /// @brief map screen coordinates to opengl coordinates
    QPointF mapToNative(const QPointF& ssPoint) const;

    QSize nativeSize() const;

    bool intersectRayWithPlane(const QVector3D& rayOrigin, const QVector3D& rayDirection, const QVector3D& planeOrigin, const QVector3D& planeNormal, QVector3D& intersectionPoint) const;
    void recomputeBBox(float radiusFactor = 1.0f) const; // radiusFactor scales the bounding box used to compute the zoom level

    Q_INVOKABLE QVector3D projectOnLine(const QPointF& ssPoint, const QVector3D& lineOrigin, const QVector3D& lineDirection) const;
    Q_INVOKABLE QVector3D projectOnPlane(const QPointF& ssPoint, const QVector3D& planeOrigin, const QVector3D& planeNormal) const;
    Q_INVOKABLE QVector4D projectOnGeometry(const QPointF& ssPoint) const;    // .w == 0 => background hit ; .w == 1 => geometry hit
	Q_INVOKABLE QVector4D projectOnGeometryWithTags(const QPointF& ssPoint, const QStringList& tags) const;    // .w == 0 => background hit ; .w == 1 => geometry hit

    Q_INVOKABLE sofaqtquick::SelectableSofaParticle*    pickParticle(const QPointF& ssPoint) const;

    Q_INVOKABLE sofaqtquick::SelectableSofaParticle*    pickParticle2(const QVector3D& origin, const QVector3D& direction) const;
    Q_INVOKABLE sofaqtquick::SelectableSofaParticle*    pickParticleWithTags(const QPointF& ssPoint, const QStringList& tags) const;

    sofaqtquick::Selectable* pickObject(const QPointF& ssPoint,
                                          const QStringList& tags,
                                          const QList<sofaqtquick::bindings::SofaBase*>& roots) ;
    Q_INVOKABLE sofaqtquick::Selectable*                pickObject(const QPointF& ssPoint) ;
    Q_INVOKABLE sofaqtquick::Selectable*                pickObjectWithTags(const QPointF& ssPoint, const QStringList& tags) ;

    Q_INVOKABLE QPair<QVector3D, QVector3D> boundingBox() const;
	Q_INVOKABLE QPair<QVector3D, QVector3D> rootsBoundingBox() const;
    Q_INVOKABLE QVector3D boundingBoxMin() const;
    Q_INVOKABLE QVector3D boundingBoxMax() const;

    Q_INVOKABLE void saveScreenshot(const QString& path);
	Q_INVOKABLE void saveScreenshotWithResolution(const QString& path, int width, int height);

    Q_INVOKABLE void saveCameraToFile(int uiId) const;
    Q_INVOKABLE void loadCameraFromFile(int uiId);

	QOpenGLFramebufferObject* getFBO() const;
    

    QQmlListProperty<sofaqtquick::Manipulator> manipulators();
    Q_INVOKABLE Manipulator* getManipulator(const QString& name);

signals:
    void sofaSceneChanged(sofaqtquick::SofaBaseScene* newScene);
    void rootsChanged(QList<sofaqtquick::bindings::SofaBase> newRoots);
    void cameraChanged(sofaqtquick::Camera* newCamera);
    void backgroundColorChanged(QColor newBackgroundColor);
    void backgroundImageSourceChanged(QUrl newBackgroundImageSource);
    void antialiasingSamplesChanged(int newAntialiasingSamples);
    void mirroredHorizontallyChanged(bool newMirroredHorizontally);
    void mirroredVerticallyChanged(bool newMirroredVertically);
    void drawFrameChanged(bool newDrawFrame);
    void drawManipulatorsChanged(bool newDrawManipulators);
    void drawSelectedChanged(bool newDrawSelected);
	void alwaysDrawChanged(bool newAlwaysDraw);
    void sceneUnitsChanged(float newSceneUnits);

    void preDraw() const;
    void postDraw() const;
public slots:
    void viewAll(float radiusFactor = 1.0f); // radiusFactor scales the bounding box used to compute the zoom level

protected:
    QSGNode* updatePaintNode(QSGNode* inOutNode, UpdatePaintNodeData* inOutData);
    virtual void internalRender(int width, int height) const;
    void renderFrame() const;

    sofa::core::visual::VisualParams* setupVisualParams(sofa::core::visual::VisualParams* visualParams) const ;
    void drawManipulator(const SofaViewer& viewer) const ;
    void drawEditorView(const QList<sofaqtquick::bindings::SofaBase*>& roots,
                        bool doDrawSelected, bool doDrawManipulators) const ;
    void drawSelectedComponents(sofa::core::visual::VisualParams* visualParams) const ;

    void drawVisuals() const ;
    void drawDebugVisuals() const ;


    void clearBuffers(const QSize& size, const QColor& color, const QImage& image=QImage()) const ;
    void setupCamera(int width, int height, const SofaViewer& viewer) const ;
    void checkAndInit() ;

private:
    QRect nativeRect() const;
    QRect qtRect() const;
    unsigned int tex;

private slots:
    void handleBackgroundImageSourceChanged(QUrl newBackgroundImageSource);

private:
    class SofaRenderer : public QQuickFramebufferObject::Renderer
    {
    public:
        SofaRenderer(SofaViewer* viewer);

    protected:
        QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
        void synchronize(QQuickFramebufferObject* quickFramebufferObject);
        void render();

    private:
        // TODO: not safe at all when we will use multithreaded rendering, use synchronize() instead
        SofaViewer* myViewer;
        int         myAntialiasingSamples;



    };

protected:
    sofa::core::visual::VisualParams* m_visualParams     {nullptr};
    QOpenGLFramebufferObject*   myFBO                    {nullptr};
    QOpenGLFramebufferObject*   myPickingFBO             {nullptr};
    QOpenGLShaderProgram*       myHighlightShaderProgram {nullptr};
    QOpenGLShaderProgram*       myPickingShaderProgram   {nullptr};
    QOpenGLShaderProgram*       myGridShaderProgram      {nullptr};
    SofaBaseScene*              mySofaScene              {nullptr};
    Camera*						myCamera                 {nullptr};
    QList<sofaqtquick::bindings::SofaBase*> myRoots;
    QColor                      myBackgroundColor;
    QUrl                        myBackgroundImageSource;
    QImage                      myBackgroundImage;
    int                         myAntialiasingSamples;
    bool                        myMirroredHorizontally;
    bool                        myMirroredVertically;
    bool                        myDrawFrame;
    bool                        myDrawManipulators;
    bool                        myDrawSelected;
	bool						myAlwaysDraw;
    bool                        myAutoPaint;
    float                       mySceneUnits;

    QList<Manipulator*>         m_manipulators;


};

}  // namespace sofaqtquick

#endif // SOFAVIEWER_H

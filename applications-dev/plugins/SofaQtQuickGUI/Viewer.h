#ifndef VIEWER_H
#define VIEWER_H

#include "SofaQtQuickGUI.h"
#include "Camera.h"

#include <QtQuick/QQuickItem>
#include <QVector3D>
#include <QVector4D>
#include <QImage>
#include <QColor>

class QOpenGLFramebufferObject;

namespace sofa
{

namespace qtquick
{

class SceneComponent;
class Scene;
class Camera;
class Manipulator;

class PickUsingRasterizationWorker;

class SOFA_SOFAQTQUICKGUI_API Viewer : public QQuickItem
{
    Q_OBJECT

    friend class PickUsingRasterizationWorker;

public:
    explicit Viewer(QQuickItem* parent = 0);
	~Viewer();

	void classBegin();
	void componentComplete();

public:
    Q_PROPERTY(sofa::qtquick::Scene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(sofa::qtquick::Camera* camera READ camera WRITE setCamera NOTIFY cameraChanged)

    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QUrl backgroundImageSource READ backgroundImageSource WRITE setBackgroundImageSource NOTIFY backgroundImageSourceChanged)
    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe NOTIFY wireframeChanged)
    Q_PROPERTY(bool culling READ culling WRITE setCulling NOTIFY cullingChanged)
    Q_PROPERTY(bool blending READ blending WRITE setBlending NOTIFY blendingChanged)
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing NOTIFY antialiasingChanged)

public:
    Scene* scene() const        {return myScene;}
    void setScene(Scene* newScene);

    Camera* camera() const      {return myCamera;}
    void setCamera(Camera* newCamera);

    QColor backgroundColor() const	{return myBackgroundColor;}
    void setBackgroundColor(QColor newBackgroundColor);

    QUrl backgroundImageSource() const	{return myBackgroundImageSource;}
    void setBackgroundImageSource(QUrl newBackgroundImageSource);

    bool wireframe() const      {return myWireframe;}
    void setWireframe(bool newWireframe);

    bool culling() const        {return myCulling;}
    void setCulling(bool newCulling);

    bool blending() const        {return myBlending;}
    void setBlending(bool newBlending);

    bool antialiasing() const        {return myAntialiasing;}
    void setAntialiasing(bool newAntialiasing);

    Q_INVOKABLE QVector3D mapFromWorld(const QVector3D& wsPoint);
    Q_INVOKABLE QVector3D mapToWorld(const QPointF& ssPoint, double z);

    QVector3D intersectRayWithPlane(const QVector3D& rayOrigin, const QVector3D& rayDirection, const QVector3D& planeOrigin, const QVector3D& planeNormal);

    Q_INVOKABLE QVector3D projectOnLine(const QPointF& ssPoint, const QVector3D& lineOrigin, const QVector3D& lineDirection);
    Q_INVOKABLE QVector3D projectOnPlane(const QPointF& ssPoint, const QVector3D& planeOrigin, const QVector3D& planeNormal);
    Q_INVOKABLE QVector4D projectOnGeometry(const QPointF& ssPoint);    // .w == 0 => background hit ; .w == 1 => geometry hit

    Q_INVOKABLE QPair<QVector3D, QVector3D> boundingBox() const;
    Q_INVOKABLE QVector3D boundingBoxMin() const;
    Q_INVOKABLE QVector3D boundingBoxMax() const;

signals:
    void sceneChanged(sofa::qtquick::Scene* newScene);
	void scenePathChanged();
    void cameraChanged(sofa::qtquick::Camera* newCamera);
    void backgroundColorChanged(QColor newBackgroundColor);
    void backgroundImageSourceChanged(QUrl newBackgroundImageSource);
    void wireframeChanged(bool newWireframe);
    void cullingChanged(bool newCulling);
    void blendingChanged(bool newBlending);
    void antialiasingChanged(bool newAntialiasing);

public slots:
    void paint();
	void viewAll();

public:
    /// \brief      Mid-level function for color index picking
    /// \note       The best way to pick an object is to use a 'PickingInteractor' instead of directly call this function
    /// \return     True if an object has been picked, false if we hit the background or a non-selectable object
    bool pickUsingRasterization(const QPointF& ssPoint, SceneComponent*& sceneComponent, Manipulator*& manipulator, QVector3D& wsPoint);

private:
	QRect glRect();
    void internalDraw();

    QPointF mapToNative(const QPointF& ssPoint);

private slots:
	void handleSceneChanged(Scene* scene);
	void handleScenePathChanged();
    void handleBackgroundImageSourceChanged(QUrl newBackgroundImageSource);
    void handleWindowChanged(QQuickWindow* window);

private:
	Scene*						myScene;
	Camera*						myCamera;
    QColor                      myBackgroundColor;
    QUrl                        myBackgroundImageSource;
    QImage                      myBackgroundImage;
    bool                        myWireframe;
    bool                        myCulling;
    bool                        myBlending;
    bool                        myAntialiasing;

};

}

}

#endif // VIEWER_H

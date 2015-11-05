#ifndef VIEWER_H
#define VIEWER_H

#include "SofaQtQuickGUI.h"
#include "Camera.h"
#include "SelectableSceneParticle.h"

#include <QtQuick/QQuickFramebufferObject>
#include <QVector3D>
#include <QVector4D>
#include <QImage>
#include <QColor>

#ifdef SOFA_HAVE_PNG
#include <sofa/helper/io/ImagePNG.h>
#else
#include <sofa/helper/io/ImageBMP.h>
#endif

namespace sofa
{

namespace qtquick
{

class SofaRenderer;
class SceneComponent;
class Scene;
class Camera;
class Manipulator;

class PickUsingRasterizationWorker;

/// @class Display a Sofa Scene in a QQuickFramebufferObject
/// @note Coordinate prefix meaning:
/// ws  => world space
/// vs  => view space
/// cs  => clip space
/// ndc => ndc space
/// ss  => screen space (window space)
class SOFA_SOFAQTQUICKGUI_API Viewer : public QQuickFramebufferObject
{
    Q_OBJECT

    friend class PickUsingRasterizationWorker;
    friend class SofaRenderer;

public:
    explicit Viewer(QQuickItem* parent = 0);
	~Viewer();

public:
    Q_PROPERTY(sofa::qtquick::Scene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(sofa::qtquick::Camera* camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(sofa::qtquick::SceneComponent* subTree READ subTree WRITE setSubTree NOTIFY subTreeChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QUrl backgroundImageSource READ backgroundImageSource WRITE setBackgroundImageSource NOTIFY backgroundImageSourceChanged)
    Q_PROPERTY(QUrl folderToSaveVideo READ folderToSaveVideo WRITE setFolderToSaveVideo NOTIFY folderToSaveVideoChanged)
    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe NOTIFY wireframeChanged)
    Q_PROPERTY(bool culling READ culling WRITE setCulling NOTIFY cullingChanged)
    Q_PROPERTY(bool blending READ blending WRITE setBlending NOTIFY blendingChanged)
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing NOTIFY antialiasingChanged)
    Q_PROPERTY(bool mirroredHorizontally READ mirroredHorizontally WRITE setMirroredHorizontally NOTIFY mirroredHorizontallyChanged)
    Q_PROPERTY(bool mirroredVertically READ mirroredVertically WRITE setMirroredVertically NOTIFY mirroredVerticallyChanged)
    Q_PROPERTY(bool drawNormals MEMBER myDrawNormals NOTIFY drawNormalsChanged)
    Q_PROPERTY(float normalsDrawLength MEMBER myNormalsDrawLength NOTIFY normalsDrawLengthChanged)
    Q_PROPERTY(bool saveVideo READ saveVideo WRITE setSaveVideo NOTIFY saveVideoChanged)

public:
    Renderer* createRenderer() const {return new SofaRenderer(const_cast<Viewer*>(this));}

    Scene* scene() const        {return myScene;}
    void setScene(Scene* newScene);

    Camera* camera() const      {return myCamera;}
    void setCamera(Camera* newCamera);

    SceneComponent* subTree() const      {return mySubTree;}
    void setSubTree(SceneComponent* newSubTree);

    QColor backgroundColor() const	{return myBackgroundColor;}
    void setBackgroundColor(QColor newBackgroundColor);

    QUrl backgroundImageSource() const	{return myBackgroundImageSource;}
    void setBackgroundImageSource(QUrl newBackgroundImageSource);

    const QUrl& folderToSaveVideo() const	{return myFolderToSaveVideo;}
    void setFolderToSaveVideo(const QUrl& newFolderToSaveVideo);

    bool wireframe() const      {return myWireframe;}
    void setWireframe(bool newWireframe);

    bool culling() const        {return myCulling;}
    void setCulling(bool newCulling);

    bool blending() const        {return myBlending;}
    void setBlending(bool newBlending);

    bool antialiasing() const        {return myAntialiasing;}
    void setAntialiasing(bool newAntialiasing);

    bool mirroredHorizontally() const        {return myMirroredHorizontally;}
    void setMirroredHorizontally(bool newMirroredHorizontally);

    bool mirroredVertically() const        {return myMirroredVertically;}
    void setMirroredVertically(bool newMirroredVertically);

    bool saveVideo() const        {return mySaveVideo;}
    void setSaveVideo(bool newSaveVideo);

    /// @return depth in screen space
    Q_INVOKABLE double computeDepth(const QVector3D& wsPosition) const;

    Q_INVOKABLE QVector3D mapFromWorld(const QVector3D& wsPoint) const;
    Q_INVOKABLE QVector3D mapToWorld(const QPointF& ssPoint, double z) const;

    QVector3D intersectRayWithPlane(const QVector3D& rayOrigin, const QVector3D& rayDirection, const QVector3D& planeOrigin, const QVector3D& planeNormal) const;

    Q_INVOKABLE QVector3D projectOnLine(const QPointF& ssPoint, const QVector3D& lineOrigin, const QVector3D& lineDirection) const;
    Q_INVOKABLE QVector3D projectOnPlane(const QPointF& ssPoint, const QVector3D& planeOrigin, const QVector3D& planeNormal) const;
    Q_INVOKABLE QVector4D projectOnGeometry(const QPointF& ssPoint) const;    // .w == 0 => background hit ; .w == 1 => geometry hit

    Q_INVOKABLE sofa::qtquick::SelectableSceneParticle*    pickParticle(const QPointF& ssPoint) const;
    Q_INVOKABLE sofa::qtquick::Selectable*                 pickObject(const QPointF& ssPoint);

    Q_INVOKABLE QPair<QVector3D, QVector3D> boundingBox() const;
    Q_INVOKABLE QVector3D boundingBoxMin() const;
    Q_INVOKABLE QVector3D boundingBoxMax() const;

    Q_INVOKABLE void takeViewerScreenshot();
    Q_INVOKABLE void saveScreenshotInFile();
    Q_INVOKABLE void saveVideoInFile(QUrl folderPath, int viewerIndex);

signals:
    void sceneChanged(sofa::qtquick::Scene* newScene);
    void subTreeChanged(sofa::qtquick::SceneComponent* newSubTree);
    void cameraChanged(sofa::qtquick::Camera* newCamera);
    void backgroundColorChanged(QColor newBackgroundColor);
    void backgroundImageSourceChanged(QUrl newBackgroundImageSource);
    void folderToSaveVideoChanged(const QUrl& newFolderToSaveVideo);
    void wireframeChanged(bool newWireframe);
    void cullingChanged(bool newCulling);
    void blendingChanged(bool newBlending);
    void antialiasingChanged(bool newAntialiasing);
    void mirroredHorizontallyChanged(bool newMirroredHorizontally);
    void mirroredVerticallyChanged(bool newMirroredVertically);
    void drawNormalsChanged(bool newDrawNormals);
    void normalsDrawLengthChanged(float newNormalsDrawLength);
    void saveVideoChanged(bool newSaveVideo);

    void preDraw();
    void postDraw();

public slots:
    void viewAll();

protected:
    QSGNode* updatePaintNode(QSGNode* inOutNode, UpdatePaintNodeData* inOutData);

private:
    QRect glRect() const;
    QRect qtRect() const;

    QPointF mapToNative(const QPointF& ssPoint) const;

private slots:
    void handleBackgroundImageSourceChanged(QUrl newBackgroundImageSource);

private:
    class SofaRenderer : public QQuickFramebufferObject::Renderer
    {
    public:
        SofaRenderer(Viewer* viewer);

    protected:
        QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
        void render();

    private:
        // TODO: not safe at all when we will use multithreaded rendering, use synchronize() instead
        Viewer* myViewer;

    };

private:
    QOpenGLFramebufferObject*   myFBO;
	Scene*						myScene;
	Camera*						myCamera;
    SceneComponent*				mySubTree;
    QColor                      myBackgroundColor;
    QUrl                        myBackgroundImageSource;
    QUrl                        myFolderToSaveVideo;
    QImage                      myBackgroundImage;
    QImage                      myScreenshotImage;
    bool                        myWireframe;
    bool                        myCulling;
    bool                        myBlending;
    bool                        myAntialiasing;
    bool                        myMirroredHorizontally;
    bool                        myMirroredVertically;
    bool                        myDrawNormals;
    float                       myNormalsDrawLength;
    bool                        mySaveVideo;
    int                         myVideoFrameCounter;

#ifdef SOFA_HAVE_PNG
    sofa::helper::io::ImagePNG myVideoFrame;
#else
    sofa::helper::io::ImageBMP myVideoFrame;
#endif

};

}

}

#endif // VIEWER_H

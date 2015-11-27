#include "Viewer.h"
#include "Scene.h"
#include "Manipulator.h"

#include <sofa/simulation/common/Node.h>
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

namespace sofa
{

namespace qtquick
{

using namespace sofa::simulation;

Viewer::Viewer(QQuickItem* parent) : QQuickFramebufferObject(parent),
    myFBO(nullptr),
    myScene(nullptr),
    myCamera(nullptr),
    myRoots(),
    myBackgroundColor("#00404040"),
    myBackgroundImageSource(),
    myBackgroundImage(),
    myWireframe(false),
    myCulling(true),
    myBlending(false),
    myAntialiasingSamples(2),
    myMirroredHorizontally(false),
    myMirroredVertically(false),
    myDrawManipulators(true),
    myDrawNormals(false),
    myNormalsDrawLength(1.0f)
{
    setFlag(QQuickItem::ItemHasContents);

    connect(this, &Viewer::backgroundImageSourceChanged, this, &Viewer::handleBackgroundImageSourceChanged);
    connect(this, SIGNAL(antialiasingSamplesChanged(int)), this, SLOT(update()));
}

Viewer::~Viewer()
{
	/*sofa::core::visual::VisualParams* _vparams = sofa::core::visual::VisualParams::defaultInstance();
	if(_vparams && _vparams->drawTool())
	{
		delete _vparams->drawTool();
		_vparams->drawTool() = 0;
	}*/

    clearRoots();
}

void Viewer::setScene(Scene* newScene)
{
	if(newScene == myScene)
		return;

	myScene = newScene;

	sceneChanged(newScene);
}

void Viewer::setCamera(Camera* newCamera)
{
	if(newCamera == myCamera)
		return;

	myCamera = newCamera;

	cameraChanged(newCamera);
}

static void appendRoot(QQmlListProperty<SceneComponent> *property, SceneComponent *value)
{
    if(value)
        static_cast<QList<SceneComponent*>*>(property->data)->append(new SceneComponent(*value));
}

static SceneComponent* atRoot(QQmlListProperty<SceneComponent> *property, int index)
{
    return static_cast<QList<SceneComponent*>*>(property->data)->at(index);
}

static void clearRoot(QQmlListProperty<SceneComponent> *property)
{
    QList<SceneComponent*>& roots = *static_cast<QList<SceneComponent*>*>(property->data);

    for(SceneComponent* sceneComponent : roots)
        delete sceneComponent;

    roots.clear();
}

static int countRoot(QQmlListProperty<SceneComponent> *property)
{
    return static_cast<QList<SceneComponent*>*>(property->data)->size();
}

QList<SceneComponent*> Viewer::roots() const
{
    return myRoots;
}

QQmlListProperty<SceneComponent> Viewer::rootsListProperty()
{
    return QQmlListProperty<SceneComponent>(this, &myRoots, appendRoot, countRoot, atRoot, clearRoot);
}

void Viewer::clearRoots()
{
    myRoots.clear();
}

void Viewer::setBackgroundColor(QColor newBackgroundColor)
{
    if(newBackgroundColor == myBackgroundColor)
        return;

    myBackgroundColor = newBackgroundColor;

    backgroundColorChanged(newBackgroundColor);
}

void Viewer::setBackgroundImageSource(QUrl newBackgroundImageSource)
{
    if(newBackgroundImageSource == myBackgroundImageSource)
        return;

    myBackgroundImageSource = newBackgroundImageSource;

    backgroundImageSourceChanged(newBackgroundImageSource);
}

void Viewer::setWireframe(bool newWireframe)
{
    if(newWireframe == myWireframe)
        return;

    myWireframe = newWireframe;

    wireframeChanged(newWireframe);
}

void Viewer::setCulling(bool newCulling)
{
    if(newCulling == myCulling)
        return;

    myCulling = newCulling;

    cullingChanged(newCulling);
}

void Viewer::setBlending(bool newBlending)
{
    if(newBlending == myBlending)
        return;

    myBlending = newBlending;

    blendingChanged(newBlending);
}

void Viewer::setAntialiasingSamples(int newAntialiasingSamples)
{
    if(newAntialiasingSamples == myAntialiasingSamples)
        return;

    myAntialiasingSamples = newAntialiasingSamples;

    antialiasingSamplesChanged(newAntialiasingSamples);
}

void Viewer::setMirroredHorizontally(bool newMirroredHorizontally)
{
    if(newMirroredHorizontally == myMirroredHorizontally)
        return;

    myMirroredHorizontally = newMirroredHorizontally;

    mirroredHorizontallyChanged(newMirroredHorizontally);
}

void Viewer::setMirroredVertically(bool newMirroredVertically)
{
    if(newMirroredVertically == myMirroredVertically)
        return;

    myMirroredVertically = newMirroredVertically;

    mirroredVerticallyChanged(newMirroredVertically);
}

void Viewer::setDrawManipulators(bool newDrawManipulators)
{
    if(newDrawManipulators == myDrawManipulators)
        return;

    myDrawManipulators = newDrawManipulators;

    drawManipulatorsChanged(newDrawManipulators);
}

void Viewer::setDrawNormals(bool newDrawNormals)
{
    if(newDrawNormals == myDrawNormals)
        return;

    myDrawNormals = newDrawNormals;

    drawNormalsChanged(newDrawNormals);
}

void Viewer::setNormalsDrawLength(float newNormalsDrawLength)
{
    if(newNormalsDrawLength == myNormalsDrawLength)
        return;

    myNormalsDrawLength = newNormalsDrawLength;

    normalsDrawLengthChanged(newNormalsDrawLength);
}

double Viewer::computeDepth(const QVector3D& wsPosition) const
{
    if(!myCamera)
        return 1.0;

    return myCamera->computeDepth(wsPosition) * 0.5 + 0.5;
}

QVector3D Viewer::mapFromWorld(const QVector3D& wsPoint) const
{
	if(!myCamera)
		return QVector3D();

    QVector4D nsPosition = (myCamera->projection() * myCamera->view() * QVector4D(wsPoint, 1.0));
	nsPosition /= nsPosition.w();

    if(mirroredHorizontally())
        nsPosition.setX(-nsPosition.x());

    if(mirroredVertically())
        nsPosition.setY(-nsPosition.y());

	return QVector3D((nsPosition.x() * 0.5 + 0.5) * qCeil(width()) + 0.5, qCeil(height()) - (nsPosition.y() * 0.5 + 0.5) * qCeil(height()) + 0.5, (nsPosition.z() * 0.5 + 0.5));
}

QVector3D Viewer::mapToWorld(const QPointF& ssPoint, double z) const
{
	if(!myCamera)
		return QVector3D();

    QVector3D nsPosition = QVector3D(ssPoint.x() / (double) qCeil(width()) * 2.0 - 1.0, (1.0 - ssPoint.y() / (double) qCeil(height())) * 2.0 - 1.0, z * 2.0 - 1.0);
    if(mirroredHorizontally())
        nsPosition.setX(-nsPosition.x());

    if(mirroredVertically())
        nsPosition.setY(-nsPosition.y());

	QVector4D vsPosition = myCamera->projection().inverted() * QVector4D(nsPosition, 1.0);
	vsPosition /= vsPosition.w();

	return (myCamera->model() * vsPosition).toVector3D();
}


bool Viewer::intersectRayWithPlane(const QVector3D& rayOrigin, const QVector3D& rayDirection, const QVector3D& planeOrigin, const QVector3D& planeNormal, QVector3D& intersectionPoint) const
{
    if(0.0 == QVector3D::dotProduct(rayDirection, planeNormal)) // ray and plane are orthogonal
        return false;

    QVector3D normalizedPlaneNormal = planeNormal.normalized();

    double d = QVector3D(0.0, 0.0, 0.0).distanceToPlane(planeOrigin, normalizedPlaneNormal);

    double nDotOrigin = QVector3D::dotProduct(normalizedPlaneNormal, rayOrigin);

    QVector3D normalizedRayDirection = rayDirection.normalized();
    double nDotDir = QVector3D::dotProduct(normalizedPlaneNormal, normalizedRayDirection);

    intersectionPoint = QVector3D(rayOrigin + ((-(nDotOrigin + d) / nDotDir) * normalizedRayDirection));

    // line intersection but not ray intersection (the intersection happened 'behind' the ray)
    if(QVector3D::dotProduct(rayDirection, intersectionPoint - rayOrigin) < 0.0)
        return false;

    return true;
}

QVector3D Viewer::projectOnLine(const QPointF& ssPoint, const QVector3D& lineOrigin, const QVector3D& lineDirection) const
{
    if(window())
    {
        QVector3D wsOrigin = mapToWorld(ssPoint, 0.0);
        QVector3D wsDirection = mapToWorld(ssPoint, 1.0) - wsOrigin;

        QVector3D normalizedLineDirection = lineDirection.normalized();
        QVector3D planAxis = QVector3D::normal(normalizedLineDirection, wsDirection);
        QVector3D planNormal = QVector3D::normal(normalizedLineDirection, planAxis);

        QVector3D intersectionPoint;
        if(intersectRayWithPlane(wsOrigin, wsDirection, lineOrigin, planNormal, intersectionPoint))
            return lineOrigin + normalizedLineDirection * QVector3D::dotProduct(normalizedLineDirection, intersectionPoint - lineOrigin);
    }

    return QVector3D(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
}

QVector3D Viewer::projectOnPlane(const QPointF& ssPoint, const QVector3D& planeOrigin, const QVector3D& planeNormal) const
{
    if(window())
    {
        QVector3D wsOrigin = mapToWorld(ssPoint, 0.0);
        QVector3D wsDirection = mapToWorld(ssPoint, 1.0) - wsOrigin;

        QVector3D intersectionPoint;
        if(intersectRayWithPlane(wsOrigin, wsDirection, planeOrigin, planeNormal, intersectionPoint))
            return intersectionPoint;
    }

    return QVector3D(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
}

using sofa::simulation::Node;
using sofa::component::visualmodel::OglModel;

class PickUsingRasterizationWorker : public QRunnable
{
public:
    PickUsingRasterizationWorker(const Viewer* viewer, const QPointF& ssPoint, Selectable*& selectable, bool& finished) :
        myViewer(viewer),
        mySSPoint(ssPoint),
        mySelectable(selectable),
        myFinished(finished)
    {

    }

    void run()
    {
        QRect rect = myViewer->nativeRect();

        QSize size = rect.size();
        if(size.isEmpty())
            return;

        Camera* camera = myViewer->camera();
        if(!camera)
            return;

        Scene* scene = myViewer->scene();
        if(!scene)
            return;

        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);

        glDisable(GL_CULL_FACE);

        glViewport(0.0f, 0.0f, size.width(), size.height());

        camera->setAspectRatio(size.width() / (double) size.height());

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(camera->projection().constData());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(camera->view().constData());

        if(myViewer->wireframe())
            glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

        if(myViewer->culling())
            glEnable(GL_CULL_FACE);

        mySelectable = scene->pickObject(*myViewer, mySSPoint, myViewer->roots());

        if(myViewer->wireframe())
            glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        myFinished = true;
    }

private:
    const Viewer*       myViewer;
    QPointF             mySSPoint;
    Selectable*&        mySelectable;
    bool&               myFinished;

};

QVector4D Viewer::projectOnGeometry(const QPointF& ssPoint) const
{
    if(!window() || !window()->isActive())
        return QVector4D();

    bool finished = false;
    Selectable* selectable = nullptr;

    PickUsingRasterizationWorker* worker = new PickUsingRasterizationWorker(this, ssPoint, selectable, finished);
    window()->scheduleRenderJob(worker, QQuickWindow::AfterSynchronizingStage);
    window()->update();

    // TODO: add a timeout
    while(!finished)
        qApp->processEvents(QEventLoop::WaitForMoreEvents);

    if(selectable)
        return QVector4D(selectable->position(), 1.0f);

    return QVector4D(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), 0.0f);
}

SelectableSceneParticle* Viewer::pickParticle(const QPointF& ssPoint) const
{
    QVector3D nearPosition = mapToWorld(ssPoint, 0.0);
    QVector3D farPosition  = mapToWorld(ssPoint, 1.0);

    QVector3D origin = nearPosition;
    QVector3D direction = (farPosition - nearPosition).normalized();

    double distanceToRay = myScene->radius() / 76.0;
    double distanceToRayGrowth = 0.001;

    return myScene->pickParticle(origin, direction, distanceToRay, distanceToRayGrowth, roots());
}

Selectable* Viewer::pickObject(const QPointF& ssPoint)
{
    Selectable* selectable = nullptr;

    if(!window() || !window()->isActive())
        return selectable;

    bool finished = false;

    PickUsingRasterizationWorker* worker = new PickUsingRasterizationWorker(this, ssPoint, selectable, finished);
    window()->scheduleRenderJob(worker, QQuickWindow::AfterSynchronizingStage);
    window()->update();

    // TODO: add a timeout
    while(!finished)
        qApp->processEvents(QEventLoop::WaitForMoreEvents);

    return selectable;
}

QPair<QVector3D, QVector3D> Viewer::boundingBox() const
{
    QVector3D min, max;
    myScene->computeBoundingBox(min, max);

    return QPair<QVector3D, QVector3D>(min, max);
}

QVector3D Viewer::boundingBoxMin() const
{
    QVector3D min, max;
    myScene->computeBoundingBox(min, max);

    return min;
}

QVector3D Viewer::boundingBoxMax() const
{
    QVector3D min, max;
    myScene->computeBoundingBox(min, max);

    return max;
}

void Viewer::handleBackgroundImageSourceChanged(QUrl newBackgroundImageSource)
{
    QString path = newBackgroundImageSource.toEncoded();
    if(path.isEmpty())
        path = newBackgroundImageSource.toLocalFile();

    myBackgroundImage = QImage(path.replace("qrc:", ":"));
}

void Viewer::saveScreenshot(const QString& path)
{    
    QDir dir = QFileInfo(path).dir();
    if(!dir.exists())
        dir.mkpath(".");

    if(!myFBO->toImage().save(path))
        qWarning() << "Screenshot could not be saved to" << path;
}

QSize Viewer::nativeSize() const
{
    return nativeRect().size();
}

QRect Viewer::nativeRect() const
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

QRect Viewer::qtRect() const
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

QPointF Viewer::mapToNative(const QPointF& ssPoint) const
{
    QPointF ssNativePoint(ssPoint);

    if(mirroredHorizontally())
        ssNativePoint.setX(width() - ssPoint.x());

    if(!mirroredVertically())
        ssNativePoint.setY(height() - ssPoint.y());

    ssNativePoint.setX(ssNativePoint.x() * window()->devicePixelRatio());
    ssNativePoint.setY(ssNativePoint.y() * window()->devicePixelRatio());

    return ssNativePoint;
}

void Viewer::viewAll()
{
	if(!myCamera || !myScene || !myScene->isReady())
		return;

	QVector3D min, max;
    myScene->computeBoundingBox(min, max);

    myCamera->fit(min, max);
}

QSGNode* Viewer::updatePaintNode(QSGNode* inOutNode, UpdatePaintNodeData* inOutData)
{
    if(!inOutNode)
    {
        inOutNode = QQuickFramebufferObject::updatePaintNode(inOutNode, inOutData);
        QSGSimpleTextureNode* textureNode = static_cast<QSGSimpleTextureNode*>(inOutNode);
        if(textureNode)
        {
            QSGSimpleTextureNode::TextureCoordinatesTransformMode mirroredHorizontallyFlag = mirroredHorizontally() ? QSGSimpleTextureNode::MirrorHorizontally : QSGSimpleTextureNode::NoTransform;
            QSGSimpleTextureNode::TextureCoordinatesTransformMode mirroredVerticallyFlag = mirroredVertically() ? QSGSimpleTextureNode::NoTransform : QSGSimpleTextureNode::MirrorVertically;

            textureNode->setTextureCoordinatesTransform(mirroredHorizontallyFlag | mirroredVerticallyFlag);
        }

        return inOutNode;
    }
    return QQuickFramebufferObject::updatePaintNode(inOutNode, inOutData);
}

Viewer::SofaRenderer::SofaRenderer(Viewer* viewer) : QQuickFramebufferObject::Renderer(),
    myViewer(viewer),
    myAntialiasingSamples(0)
{

}

QOpenGLFramebufferObject* Viewer::SofaRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(myAntialiasingSamples);

    myViewer->myFBO = new QOpenGLFramebufferObject(size, format);
    return myViewer->myFBO;
}

void Viewer::SofaRenderer::synchronize(QQuickFramebufferObject* quickFramebufferObject)
{
    Viewer* viewer = qobject_cast<Viewer*>(quickFramebufferObject);
    if(!viewer)
        return;

    if(myAntialiasingSamples != viewer->myAntialiasingSamples)
    {
        myAntialiasingSamples = viewer->myAntialiasingSamples;
        invalidateFramebufferObject();
    }
}

void Viewer::SofaRenderer::render()
{
    update();

    if(!myViewer || !myViewer->isVisible())
        return;

    QSize size(myViewer->width(), myViewer->height());
    if(size.isEmpty())
        return;

//    if(!myBackgroundImage.isNull())
//    {
//        // TODO: warning: disable lights, but why ?
//        QOpenGLPaintDevice device(size);
//        QPainter painter(&device);
//        painter.drawImage(size.width() - myBackgroundImage.width(), size.height() - myBackgroundImage.height(), myBackgroundImage);
//    }

    glClearColor(myViewer->myBackgroundColor.redF(), myViewer->myBackgroundColor.greenF(), myViewer->myBackgroundColor.blueF(), myViewer->myBackgroundColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!myViewer->myCamera)
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

    if(myViewer->blending())
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    if(myViewer->myScene && myViewer->myScene->isReady())
    {
        glDisable(GL_CULL_FACE);

        myViewer->myCamera->setAspectRatio(myViewer->width() / myViewer->height());

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(myViewer->myCamera->projection().constData());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(myViewer->myCamera->view().constData());

        if(myViewer->wireframe())
            glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

        if(myViewer->culling())
            glEnable(GL_CULL_FACE);

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
            _vparams->sceneBBox() = myViewer->myScene->sofaSimulation()->GetRoot()->f_bbox.getValue();
            _vparams->setProjectionMatrix(_projmatrix);
            _vparams->setModelViewMatrix(_mvmatrix);
        }

        // draw the sofa scene
        {
            myViewer->preDraw();
            myViewer->myScene->draw(*myViewer, myViewer->roots());
            myViewer->postDraw();
        }

        if(myViewer->wireframe())
            glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    if(myViewer->blending())
        glDisable(GL_BLEND);
}

}

}

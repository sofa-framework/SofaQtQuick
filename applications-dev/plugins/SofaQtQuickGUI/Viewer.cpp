#include "Viewer.h"
#include "Scene.h"
#include "Manipulator.h"

#include <sofa/simulation/common/Node.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <SofaOpenglVisual/OglModel.h>

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
#include <QVector>
#include <QVector4D>
#include <QTime>
#include <QPair>
#include <QThread>
#include <QString>
#include <QDir>
#include <qqml.h>
#include <qmath.h>

#include <iomanip>
#include <sstream>

namespace sofa
{

namespace qtquick
{

Viewer::Viewer(QQuickItem* parent) : QQuickItem(parent),
    myScene(nullptr),
    myCamera(nullptr),
    mySubTree(nullptr),
    myBackgroundColor("#00404040"),
    myBackgroundImageSource(),
    myFolderToSaveVideo(""),
    myBackgroundImage(),
    myScreenshotImage(),
    myWireframe(false),
    myCulling(true),
    myBlending(false),
    myAntialiasing(false),
    myDrawNormals(false),
    myNormalsDrawLength(1.0f),
    mySaveVideo(false),
    myVideoFrameCounter(0)
{
    setFlag(QQuickItem::ItemHasContents);

	connect(this, &Viewer::sceneChanged, this, &Viewer::handleSceneChanged);
	connect(this, &Viewer::scenePathChanged, this, &Viewer::handleScenePathChanged);
    connect(this, &Viewer::backgroundImageSourceChanged, this, &Viewer::handleBackgroundImageSourceChanged);
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

Viewer::~Viewer()
{
	/*sofa::core::visual::VisualParams* _vparams = sofa::core::visual::VisualParams::defaultInstance();
	if(_vparams && _vparams->drawTool())
	{
		delete _vparams->drawTool();
		_vparams->drawTool() = 0;
	}*/
}

void Viewer::classBegin()
{
    // TODO: implicit assignation => to be removed
	if(!myScene)
	{
		QQmlContext* context = QQmlEngine::contextForObject(this);
		if(context)
		{
			QVariant sceneVariant = context->contextProperty("scene");
			if(sceneVariant.canConvert(QMetaType::QObjectStar))
			{
				Scene* scene = qobject_cast<Scene*>(sceneVariant.value<QObject*>());
				if(scene)
					setScene(scene);
			}
		}
	}
}

void Viewer::componentComplete()
{

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

void Viewer::setSubTree(SceneComponent* newSubTree)
{
    if(newSubTree == mySubTree)
        return;

    mySubTree = newSubTree;

    subTreeChanged(newSubTree);
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

void Viewer::setFolderToSaveVideo(const QUrl& newFolderToSaveVideo)
{
    if(newFolderToSaveVideo == myFolderToSaveVideo)
        return;

    myFolderToSaveVideo = newFolderToSaveVideo;

    folderToSaveVideoChanged(newFolderToSaveVideo);
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


void Viewer::setAntialiasing(bool newAntialiasing)
{
    if(newAntialiasing == myAntialiasing)
        return;

    myAntialiasing = newAntialiasing;

    antialiasingChanged(newAntialiasing);
}

void Viewer::setSaveVideo(bool newSaveVideo)
{
    if(newSaveVideo == mySaveVideo)
        return;

    mySaveVideo = newSaveVideo;

    saveVideoChanged(newSaveVideo);
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

	return QVector3D((nsPosition.x() * 0.5 + 0.5) * qCeil(width()) + 0.5, qCeil(height()) - (nsPosition.y() * 0.5 + 0.5) * qCeil(height()) + 0.5, (nsPosition.z() * 0.5 + 0.5));
}

QVector3D Viewer::mapToWorld(const QPointF& ssPoint, double z) const
{
	if(!myCamera)
		return QVector3D();

    QVector3D nsPosition = QVector3D(ssPoint.x() / (double) qCeil(width()) * 2.0 - 1.0, (1.0 - ssPoint.y() / (double) qCeil(height())) * 2.0 - 1.0, z * 2.0 - 1.0);
	QVector4D vsPosition = myCamera->projection().inverted() * QVector4D(nsPosition, 1.0);
	vsPosition /= vsPosition.w();

	return (myCamera->model() * vsPosition).toVector3D();
}

class ProjectOnGeometryWorker : public QRunnable
{
public:
    ProjectOnGeometryWorker(QPointF position, float& z, bool& finished) :
        myPosition(position),
        myZ(z),
        myFinished(finished)
    {

    }

    void run()
    {
        glReadPixels(myPosition.x(), myPosition.y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &myZ);

        myFinished = true;
    }

private:
    QPointF myPosition;
    float&  myZ;
    bool&   myFinished;

};

QVector3D Viewer::intersectRayWithPlane(const QVector3D& rayOrigin, const QVector3D& rayDirection, const QVector3D& planeOrigin, const QVector3D& planeNormal) const
{
    QVector3D normalizedRayDirection = rayDirection.normalized();
    QVector3D normalizedPlaneNormal = planeNormal.normalized();

    double d = -QVector3D(0.0, 0.0, 0.0).distanceToPlane(planeOrigin, normalizedPlaneNormal);
    double nDotP0 = QVector3D::dotProduct(normalizedPlaneNormal, rayOrigin);
    double nDotDir = QVector3D::dotProduct(normalizedPlaneNormal, normalizedRayDirection);

    return rayOrigin + (((d - nDotP0) / nDotDir) * normalizedRayDirection);
}

QVector3D Viewer::projectOnLine(const QPointF& ssPoint, const QVector3D& lineOrigin, const QVector3D& lineDirection) const
{
    if(!window())
        return QVector3D();

    QVector3D wsOrigin = mapToWorld(ssPoint, 0.0);
    QVector3D wsDirection = mapToWorld(ssPoint, 1.0) - wsOrigin;

    QVector3D normalizedLineDirection = lineDirection.normalized();
    QVector3D planAxis = QVector3D::normal(normalizedLineDirection, wsDirection);
    QVector3D planNormal = QVector3D::normal(normalizedLineDirection, planAxis);

    QVector3D intersectionPoint = intersectRayWithPlane(wsOrigin, wsDirection, lineOrigin, planNormal);
    QVector3D projectedPoint = lineOrigin + normalizedLineDirection * QVector3D::dotProduct(normalizedLineDirection, intersectionPoint - lineOrigin);

    return projectedPoint;
}

QVector3D Viewer::projectOnPlane(const QPointF& ssPoint, const QVector3D& planeOrigin, const QVector3D& planeNormal) const
{
    if(!window())
        return QVector3D();

    QVector3D wsOrigin = mapToWorld(ssPoint, 0.0);
    QVector3D wsDirection = mapToWorld(ssPoint, 1.0) - wsOrigin;

    return intersectRayWithPlane(wsOrigin, wsDirection, planeOrigin, planeNormal);
}

QVector4D Viewer::projectOnGeometry(const QPointF& ssPoint) const
{
    if(!window() || !window()->isActive())
        return QVector4D();

    QPointF ssPointGL = mapToNative(ssPoint);

    float z = 1.0;
    bool finished = false;

    ProjectOnGeometryWorker* worker = new ProjectOnGeometryWorker(ssPointGL, z, finished);
    window()->scheduleRenderJob(worker, QQuickWindow::BeforeRenderingStage);
    window()->update();

    // TODO: add a timeout
    while(!finished)
        qApp->processEvents(QEventLoop::WaitForMoreEvents | QEventLoop::ExcludeUserInputEvents);

    return QVector4D(mapToWorld(ssPoint, z), qCeil(1.0f - z));
}

SelectableSceneParticle* Viewer::pickParticle(const QPointF& ssPoint) const
{
    QVector3D nearPosition = mapToWorld(ssPoint, 0.0);
    QVector3D farPosition  = mapToWorld(ssPoint, 1.0);

    QVector3D origin = nearPosition;
    QVector3D direction = (farPosition - nearPosition).normalized();

    double distanceToRay = myScene->radius() / 76.0;
    double distanceToRayGrowth = 0.001;

    return myScene->pickParticle(origin, direction, distanceToRay, distanceToRayGrowth);
}

using sofa::simulation::Node;
using sofa::component::visualmodel::OglModel;

class PickUsingRasterizationWorker : public QRunnable
{
public:
    PickUsingRasterizationWorker(Scene* scene, Viewer* viewer, QPointF nativePoint, Selectable*& selectable, float& z, bool& finished) :
        myScene(scene),
        myViewer(viewer),
        myPosition(nativePoint),
        mySelectable(selectable),
        myZ(z),
        myFinished(finished)
    {

    }

    void run()
    {
        QRect rect = myViewer->glRect();

        QPoint pos = rect.topLeft();
        QSize size = rect.size();
        if(size.isEmpty())
            return;

        Camera* camera = myViewer->camera();
        if(!camera)
            return;

        // clear the viewer rectangle and just its area, not the whole OpenGL buffer
        glScissor(pos.x(), pos.y(), size.width(), size.height());
        glEnable(GL_SCISSOR_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);

        glViewport(pos.x(), pos.y(), size.width(), size.height());

        glDisable(GL_CULL_FACE);

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

        mySelectable = myScene->pickObject(*myViewer, myPosition);

        if(mySelectable)
            glReadPixels(myPosition.x(), myPosition.y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &myZ);

        if(myViewer->wireframe())
            glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        myFinished = true;
    }

private:
    Scene*              myScene;
    Viewer*             myViewer;
    QPointF             myPosition;
    Selectable*&        mySelectable;
    float&              myZ;
    bool&               myFinished;

};

Selectable* Viewer::pickObject(const QPointF& ssPoint)
{
    Selectable* selectable = nullptr;

    if(!window() || !window()->isActive())
        return selectable;

    float z = 1.0;
    bool finished = false;

    PickUsingRasterizationWorker* worker = new PickUsingRasterizationWorker(myScene, this, mapToNative(ssPoint), selectable, z, finished);
    window()->scheduleRenderJob(worker, QQuickWindow::AfterSynchronizingStage);
    window()->update();

    // TODO: add a timeout
    while(!finished)
        qApp->processEvents(QEventLoop::WaitForMoreEvents | QEventLoop::ExcludeUserInputEvents);

    if(selectable)
        selectable->setPosition(mapToWorld(ssPoint, z));

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

void Viewer::handleSceneChanged(Scene* scene)
{
	if(scene)
    {
		if(scene->isReady())
			scenePathChanged();

		connect(scene, &Scene::loaded, this, &Viewer::scenePathChanged);
	}
}

void Viewer::handleScenePathChanged()
{
	if(!myScene || !myScene->isReady())
		return;

	viewAll();
}

void Viewer::handleBackgroundImageSourceChanged(QUrl newBackgroundImageSource)
{
    QString path = newBackgroundImageSource.toEncoded();
    if(path.isEmpty())
        path = newBackgroundImageSource.toLocalFile();

    myBackgroundImage = QImage(path.replace("qrc:", ":"));
}

void Viewer::handleWindowChanged(QQuickWindow* window)
{
    if(window)
    {
        window->setClearBeforeRendering(false);
        connect(window, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);
    }
}

void Viewer::takeViewerScreenshot()
{
    QRect rect = qtRect();
    QPoint pos = rect.topLeft();
    QSize size = rect.size();

    // Take window screenshot
    QImage screenshot =  window()->grabWindow();

    // Resize screenshot to the viewer
    myScreenshotImage = screenshot.copy(pos.x(),pos.y()-size.height(),size.width(),size.height());
}

void Viewer::saveScreenshotInFile()
{
    QString finalFilename = myScene->screenshotFilename().toLocalFile();
    if(finalFilename.isEmpty())
    {
        std::cerr << "File to save screenshot doesn't exist" << std::endl;
        return;
    }

    if(myScreenshotImage.height()!=0)
    {
        #ifdef SOFA_HAVE_PNG
            myScreenshotImage.save(finalFilename,"png");
        #else
            myScreenshotImage.save(finalFilename,"bmp");
        #endif
        std::cout << "Saved "<< myScreenshotImage.width() <<"x"<< myScreenshotImage.height() <<" viewer screen image to "<< finalFilename.toStdString() <<std::endl;
    }
}

void Viewer::saveVideoInFile(QUrl folderPath, int viewerIndex)
{
    // Define folder path to save video
    QString folderPathString = folderPath.toLocalFile();

    // If no folder path is selected take by default screenshots folder in build directory
    if(folderPathString.isEmpty())
    {
        QDir dir;

        folderPathString = dir.currentPath().replace("/bin","")+"/screenshots";
    }

    // Check folder exists
    QDir dirFolder (folderPathString);
    if(!dirFolder.exists())
    {
        std::cout << "Folder = " << folderPathString.toStdString() << "doesn't exist" << std::endl;
        return;
    }

    // Define file name to save the different video frames
    QString finalFilename = folderPathString + "/"+ myScene->source().toString().replace("file:", "").remove( QRegExp( "(.*/)" ) ).replace(".scn","");

    // Number of the frame
    std::ostringstream ss;
    ss << std::setw( 8 ) << std::setfill( '0' ) << myVideoFrameCounter;
    std::string result = ss.str();
    finalFilename = finalFilename + "Viewer" +  QString::fromStdString(std::to_string(viewerIndex)) +  QString::fromStdString("_" + result);

    // Take frame screenshot
    QRect rect = glRect();
    QPoint pos = rect.topLeft();
    QSize size = rect.size();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    myVideoFrame.init(size.width(),size.height(), 1, 1, helper::io::Image::UNORM8, helper::io::Image::RGB);
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(pos.x(),pos.y(), size.width(), size.height(), GL_RGB, GL_UNSIGNED_BYTE, myVideoFrame.getPixels());

    // Save frame image
    if(myVideoFrame.getHeight()!=0)
    {
        #ifdef SOFA_HAVE_PNG
            finalFilename = finalFilename + ".png";
        #else
            finalFilename = finalFilename + ".bmp";
        #endif
        std::string filepath = finalFilename.toLatin1().constData();
        if (!myVideoFrame.save(filepath)) return;
            std::cout << "Saved "<<myVideoFrame.getWidth()<<"x"<<myVideoFrame.getHeight()<<" screen image to "<<filepath<<std::endl;
    }

    glReadBuffer(GL_BACK);

    // Frame counter
    myVideoFrameCounter++;
}

QRect Viewer::glRect() const
{
	if(!window())
		return QRect();

    QPointF realPos(mapToNative(QPointF(0.0, qCeil(height()))));

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

void Viewer::internalDraw()
{
    if(!myScene || !myScene->isReady() || !myCamera)
        return;

    glDisable(GL_CULL_FACE);

    QSize size = glRect().size();
    myCamera->setAspectRatio(size.width() / (double) size.height());

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(myCamera->projection().constData());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(myCamera->view().constData());

     if(wireframe())
        glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
     else
         glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

    if(culling())
        glEnable(GL_CULL_FACE);

//      if(antialiasing())
//              glEnable(GL_MULTISAMPLE);

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
        _vparams->sceneBBox() = myScene->sofaSimulation()->GetRoot()->f_bbox.getValue();
        _vparams->setProjectionMatrix(_projmatrix);
        _vparams->setModelViewMatrix(_mvmatrix);
    }

    bool drawNormalBackup = myScene->drawNormals();
    float normalDrawLengthBackup = myScene->normalsDrawLength();

    myScene->setDrawNormals(myDrawNormals);
    myScene->setNormalsDrawLength(myNormalsDrawLength);

    myScene->draw(*this, mySubTree);

    myScene->setDrawNormals(drawNormalBackup);
    myScene->setNormalsDrawLength(normalDrawLengthBackup);

    if(wireframe())
        glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

QPointF Viewer::mapToNative(const QPointF& ssPoint) const
{
    QPointF ssNativePoint = mapToScene(ssPoint);
    ssNativePoint.setX(ssNativePoint.x() * window()->devicePixelRatio());
    ssNativePoint.setY((window()->height() - ssNativePoint.y()) * window()->devicePixelRatio());  // OpenGL has its Y coordinate inverted compared to Qt

    return ssNativePoint;
}

void Viewer::paint()
{
    if(!window())
        return;

    // compute the correct viewer position and size
    QRect rect = glRect();

    QPoint pos = rect.topLeft();
    QSize size = rect.size();
    if(size.isEmpty())
        return;

    // clear the viewer rectangle and just its area, not the whole OpenGL buffer
    glScissor(pos.x(), pos.y(), size.width(), size.height());
    glEnable(GL_SCISSOR_TEST);
	glClearColor(myBackgroundColor.redF(), myBackgroundColor.greenF(), myBackgroundColor.blueF(), myBackgroundColor.alphaF());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);


//    if(!myBackgroundImage.isNull())
//    {
//        // TODO: warning: disable lights, but why ?
//        QOpenGLPaintDevice device(size);
//        QPainter painter(&device);
//        painter.drawImage(size.width() - myBackgroundImage.width(), size.height() - myBackgroundImage.height(), myBackgroundImage);
//    }

    if(!myCamera)
        return;

	glViewport(pos.x(), pos.y(), size.width(), size.height());

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

	if(blending())
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	internalDraw();

	if(blending())
		glDisable(GL_BLEND);

    window()->update();
}

void Viewer::viewAll()
{
	if(!myCamera || !myScene || !myScene->isReady())
		return;

	QVector3D min, max;
    myScene->computeBoundingBox(min, max);

	myCamera->fit(min, max);
}

}

}

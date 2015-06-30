#include "Viewer.h"
#include "Scene.h"
#include "Camera.h"

#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/DrawToolGL.h>

#include <QtQuick/qquickwindow.h>
#include <QQmlEngine>
#include <QQmlContext>
//#include <QOpenGLContext>
//#include <QOpenGLPaintDevice>
//#include <QPaintEngine>
//#include <QPainter>
#include <QVector>
#include <QVector4D>
#include <QOpenGLFramebufferObject>
#include <QTime>
#include <QPair>
#include <QThread>
#include <qqml.h>
#include <qmath.h>

namespace sofa
{

namespace qtquick
{

Viewer::Viewer(QQuickItem* parent) : QQuickItem(parent),
	myScene(0),
    myCamera(0),
    myBackgroundColor("#00404040"),
    myBackgroundImageSource(),
    myBackgroundImage(),
    myWireframe(false),
    myCulling(false),
    myBlending(false),
    myAntialiasing(false),
    myFBO(0)
{
    setFlag(QQuickItem::ItemHasContents);

	connect(this, &Viewer::sceneChanged, this, &Viewer::handleSceneChanged);
	connect(this, &Viewer::scenePathChanged, this, &Viewer::handleScenePathChanged);
    connect(this, &Viewer::backgroundImageSourceChanged, this, &Viewer::handleBackgroundImageSourceChanged);
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

Viewer::~Viewer()
{
    //delete myFBO;

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


void Viewer::setAntialiasing(bool newAntialiasing)
{
    if(newAntialiasing == myAntialiasing)
        return;

    myAntialiasing = newAntialiasing;

    antialiasingChanged(newAntialiasing);
}

QVector3D Viewer::mapFromWorld(const QVector3D& wsPoint)
{
	if(!myCamera)
		return QVector3D();

    QVector4D nsPosition = (myCamera->projection() * myCamera->view() * QVector4D(wsPoint, 1.0));
	nsPosition /= nsPosition.w();

	return QVector3D((nsPosition.x() * 0.5 + 0.5) * qCeil(width()) + 0.5, qCeil(height()) - (nsPosition.y() * 0.5 + 0.5) * qCeil(height()) + 0.5, (nsPosition.z() * 0.5 + 0.5));
}

QVector3D Viewer::mapToWorld(const QVector3D& ssPoint)
{
	if(!myCamera)
		return QVector3D();

    QVector3D nsPosition = QVector3D(ssPoint.x() / (double) qCeil(width()) * 2.0 - 1.0, (1.0 - ssPoint.y() / (double) qCeil(height())) * 2.0 - 1.0, ssPoint.z() * 2.0 - 1.0);
	QVector4D vsPosition = myCamera->projection().inverted() * QVector4D(nsPosition, 1.0);
	vsPosition /= vsPosition.w();

	return (myCamera->model() * vsPosition).toVector3D();
}

QVector4D Viewer::projectOnGeometry(const QPointF& ssPoint)
{
    if(!window())
        return QVector4D();

    QPointF ssPointGL = ssPoint;
    ssPointGL.setX(ssPointGL.x() * window()->devicePixelRatio());
    ssPointGL.setY((height() - ssPointGL.y()) * window()->devicePixelRatio());  // OpenGL has its Y coordinate inverted compared to Qt

	QSize size = glRect().size();

	// TODO: use a custom fbo without color, only depth
	if(!myFBO ||
		size.width() > myFBO->width() || size.height() > myFBO->height() ||
		myFBO->width() >= size.width() / 2 || myFBO->height() >= size.height() / 2)
	{
		delete myFBO;
		myFBO = new QOpenGLFramebufferObject(size);
		myFBO->setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	}

	if(!myFBO->isValid())
	{
		qWarning() << "ERROR: cannot bind fbo to draw the Sofa scene";
		return QVector4D();
	}

	// draw only the depth in the fbo
	myFBO->bind();

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, size.width(), size.height());

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	internalDraw();

    float z;
    glReadPixels(ssPointGL.x(), ssPointGL.y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

	myFBO->release();

    return QVector4D(mapToWorld(QVector3D(ssPoint.x(), ssPoint.y(), z)), qCeil(1.0f - z));
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

QRect Viewer::glRect()
{
	if(!window())
		return QRect();

	QPointF realPos(mapToScene(QPointF(0.0, height())));
	realPos.setX(realPos.x() * window()->devicePixelRatio());
	realPos.setY((window()->height() - realPos.y()) * window()->devicePixelRatio());  // OpenGL has its Y coordinate inverted compared to Qt

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
	myCamera->setPerspectiveAspectRatio(size.width() / (double) size.height());

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(myCamera->projection().constData());

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(myCamera->view().constData());

	QVector3D cameraPosition(myCamera->eye());
	float lightPosition[] = { cameraPosition.x(), cameraPosition.y(), cameraPosition.z(), 1.0f};
	float lightAmbient [] = { 0.0f, 0.0f, 0.0f, 0.0f};
	float lightDiffuse [] = { 1.0f, 1.0f, 1.0f, 0.0f};
	float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 0.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glEnable(GL_LIGHT0);

    if(wireframe())
        glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

	if(culling())
		glEnable(GL_CULL_FACE);

// 	if(antialiasing())
// 		glEnable(GL_MULTISAMPLE);

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

	myScene->draw();

    if(wireframe())
        glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
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

    if(myScene && myScene->isLoading())
        myScene->initGraphics();

//    if(!myBackgroundImage.isNull())
//    {
//        // TODO: warning: disable lights, but why ?
//        QOpenGLPaintDevice device(size);
//        QPainter painter(&device);
//        painter.drawImage(size.width() - myBackgroundImage.width(), size.height() - myBackgroundImage.height(), myBackgroundImage);
//    }

	glViewport(pos.x(), pos.y(), size.width(), size.height());

	glEnable(GL_LIGHTING);

	if(blending())
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	internalDraw();

	if(blending())
		glDisable(GL_BLEND);
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

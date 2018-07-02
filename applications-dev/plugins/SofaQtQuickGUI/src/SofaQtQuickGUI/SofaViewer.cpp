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

#include "SofaViewer.h"
#include "SofaScene.h"
#include "Manipulators/Manipulator.h"

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
#include <QOpenGLPaintDevice>
#include <QPaintEngine>
#include <QPainter>
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
#include <QSettings>
#include <qqml.h>
#include <qmath.h>
#include <limits>

#include <iomanip>
#include <sstream>

#include <sofa/core/visual/VisualModel.h>
using sofa::core::visual::VisualModel;

#include <SofaMeshCollision/TriangleModel.h>
using sofa::component::collision::TriangleModel;

#include <SofaQtQuickGUI/Rendering/ObjectRenderer.h>
using sofa::qtquick::ObjectRenderer;

#include <SofaBaseVisual/VisualStyle.h>
using sofa::component::visualmodel::VisualStyle;

#include "SelectableManipulator.h"

#include <SofaQtQuickGUI/SofaApplication.h>

namespace sofa
{

namespace qtquick
{

using namespace sofa::simulation;

SofaViewer::SofaViewer(QQuickItem* parent) : QQuickFramebufferObject(parent),
    myFBO(nullptr),
    mySofaScene(nullptr),
    myCamera(nullptr),
    myRoots(),
    myBackgroundColor("#00404040"),
    myBackgroundImageSource(),
    myBackgroundImage(),
    myAntialiasingSamples(2),
    myMirroredHorizontally(false),
    myMirroredVertically(false),
    myDrawFrame(false),
    myDrawManipulators(true),
    myDrawSelected(true),
    myAlwaysDraw(false),
    myAutoPaint(true)
{
    QQuickFramebufferObject::setMirrorVertically(true);

    setFlag(QQuickItem::ItemHasContents);

    connect(this, &SofaViewer::backgroundImageSourceChanged, this, &SofaViewer::handleBackgroundImageSourceChanged);
    connect(this, SIGNAL(antialiasingSamplesChanged(int)), this, SLOT(update()));

    m_visualParams = new sofa::core::visual::VisualParams() ;
    m_visualParams->drawTool() = new sofa::core::visual::DrawToolGL();
    m_visualParams->setSupported(sofa::core::visual::API_OpenGL);

    sofa::qtquick::SofaApplication::InitOpenGL();
}

SofaViewer::~SofaViewer()
{
    delete m_visualParams->drawTool() ;
    delete m_visualParams;

    clearRoots();
}

QOpenGLFramebufferObject* SofaViewer::getFBO() const
{
	return myFBO;
}

void SofaViewer::setSofaScene(SofaScene* newSofaScene)
{
    if(newSofaScene == mySofaScene)
        return;

    mySofaScene = newSofaScene;

    sofaSceneChanged(newSofaScene);
}

void SofaViewer::setCamera(Camera* newCamera)
{
    if(newCamera == myCamera)
        return;

    myCamera = newCamera;

    cameraChanged(newCamera);
}

void SofaViewer::setAlwaysDraw(bool newChoiceAllwaysDraw)
{
	if (newChoiceAllwaysDraw == myAlwaysDraw)
		return;

	myAlwaysDraw = newChoiceAllwaysDraw;

	alwaysDrawChanged(newChoiceAllwaysDraw);
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

QList<SofaComponent*> SofaViewer::roots() const
{
    return myRoots;
}

QQmlListProperty<SofaComponent> SofaViewer::rootsListProperty()
{
    return QQmlListProperty<SofaComponent>(this, &myRoots, appendRoot, countRoot, atRoot, clearRoot);
}

void SofaViewer::clearRoots()
{
    myRoots.clear();
}

void SofaViewer::setBackgroundColor(QColor newBackgroundColor)
{
    if(newBackgroundColor == myBackgroundColor)
        return;

    myBackgroundColor = newBackgroundColor;

    backgroundColorChanged(newBackgroundColor);
}

void SofaViewer::setBackgroundImageSource(QUrl newBackgroundImageSource)
{
    if(newBackgroundImageSource == myBackgroundImageSource)
        return;

    myBackgroundImageSource = newBackgroundImageSource;

    backgroundImageSourceChanged(newBackgroundImageSource);
}

void SofaViewer::setAntialiasingSamples(int newAntialiasingSamples)
{
    if(newAntialiasingSamples == myAntialiasingSamples)
        return;

    myAntialiasingSamples = newAntialiasingSamples;

    antialiasingSamplesChanged(newAntialiasingSamples);
}

void SofaViewer::setMirroredHorizontally(bool newMirroredHorizontally)
{
    if(newMirroredHorizontally == myMirroredHorizontally)
        return;

    myMirroredHorizontally = newMirroredHorizontally;

    mirroredHorizontallyChanged(newMirroredHorizontally);
}

void SofaViewer::setMirroredVertically(bool newMirroredVertically)
{
    if(newMirroredVertically == myMirroredVertically)
        return;

    myMirroredVertically = newMirroredVertically;

    mirroredVerticallyChanged(newMirroredVertically);
}

void SofaViewer::setDrawFrame(bool newDrawFrame)
{
    if(newDrawFrame == myDrawFrame)
        return;

    myDrawFrame = newDrawFrame;

    drawFrameChanged(newDrawFrame);
}

void SofaViewer::setDrawManipulators(bool newDrawManipulators)
{
    if(newDrawManipulators == myDrawManipulators)
        return;

    myDrawManipulators = newDrawManipulators;

    drawManipulatorsChanged(newDrawManipulators);
}

void SofaViewer::setDrawSelected(bool newDrawSelected)
{
    if(newDrawSelected == myDrawSelected)
        return;

    myDrawSelected = newDrawSelected;

    drawSelectedChanged(newDrawSelected);
}

double SofaViewer::computeDepth(const QVector3D& wsPosition) const
{
    if(!myCamera)
        return 1.0;

    return myCamera->computeDepth(wsPosition) * 0.5 + 0.5;
}

QVector3D SofaViewer::mapFromWorld(const QVector3D& wsPoint) const
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

QVector3D SofaViewer::mapToWorld(const QPointF& ssPoint, double z) const
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


bool SofaViewer::intersectRayWithPlane(const QVector3D& rayOrigin, const QVector3D& rayDirection, const QVector3D& planeOrigin, const QVector3D& planeNormal, QVector3D& intersectionPoint) const
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

QVector3D SofaViewer::projectOnLine(const QPointF& ssPoint, const QVector3D& lineOrigin, const QVector3D& lineDirection) const
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

QVector3D SofaViewer::projectOnPlane(const QPointF& ssPoint, const QVector3D& planeOrigin, const QVector3D& planeNormal) const
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
    PickUsingRasterizationWorker(const SofaViewer* viewer, const QPointF& ssPoint, const QStringList& tags, Selectable*& selectable, bool& finished) :
        myViewer(viewer),
        mySSPoint(ssPoint),
        myTags(tags),
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

        SofaScene* sofaScene = myViewer->sofaScene();
        if(!sofaScene)
            return;

        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);

        glViewport(0.0, 0.0, size.width(), size.height());

        camera->setAspectRatio(size.width() / (double) size.height());

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(camera->projection().constData());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(camera->view().constData());

        //mySelectable = myViewer->pickObject(mySSPoint, myTags, myViewer->roots());

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        myFinished = true;
    }

private:
    const SofaViewer*   myViewer;
    QPointF             mySSPoint;
    QStringList			myTags;
    Selectable*&        mySelectable;
    bool&               myFinished;

};

void SofaViewer::checkAndInit()
{
    // init the highlight shader program
    if(!myHighlightShaderProgram)
    {
        myHighlightShaderProgram = new QOpenGLShaderProgram();

        myHighlightShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                                     "void main(void)\n"
                                                                     "{\n"
                                                                     "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
                                                                     "}");
        myHighlightShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                                                     "void main(void)\n"
                                                                     "{\n"
                                                                     "   gl_FragColor = vec4(0.75, 0.5, 0.0, 1.0);\n"
                                                                     "}");

        myHighlightShaderProgram->link();

        myHighlightShaderProgram->moveToThread(thread());
        myHighlightShaderProgram->setParent(this);
    }

    // init the picking shader program
    if(!myPickingShaderProgram)
    {
        myPickingShaderProgram = new QOpenGLShaderProgram();
        myPickingShaderProgram->create();
        myPickingShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                                   "void main(void)\n"
                                                                   "{\n"
                                                                   "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
                                                                   "}");
        myPickingShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                                                   "uniform highp vec4 index;\n"
                                                                   "void main(void)\n"
                                                                   "{\n"
                                                                   "   gl_FragColor = index;\n"
                                                                   "}");
        myPickingShaderProgram->link();

        myPickingShaderProgram->moveToThread(thread());
        myPickingShaderProgram->setParent(this);
    }

    /*
    sofa::core::visual::VisualParams* visualParams = sofa::core::visual::VisualParams::defaultInstance();
    if(visualParams)
    {
        if(!visualParams->drawTool())
        {
            visualParams->drawTool() = new sofa::core::visual::DrawToolGL();
            visualParams->setSupported(sofa::core::visual::API_OpenGL);
        }

        visualParams->displayFlags().setShowVisualModels(true);
    }
    */
}

void SofaViewer::drawVisuals() const
{
    if(!mySofaScene->isReady())
        return;

    setupVisualParams(getVisualParams());

    mySofaScene->prepareSceneForDrawing();
    mySofaScene->mySofaSimulation->draw(getVisualParams(), mySofaScene->mySofaRootNode.get());
}

void SofaViewer::drawDebugVisuals() const
{
    if(!mySofaScene->isReady())
        return;

    setupVisualParams(getVisualParams());

    mySofaScene->prepareSceneForDrawing();
    mySofaScene->mySofaSimulation->draw(getVisualParams(), mySofaScene->mySofaRootNode.get());
}


QVector4D SofaViewer::projectOnGeometry(const QPointF& ssPoint) const
{
    return projectOnGeometryWithTags(ssPoint, QStringList());
}

QVector4D SofaViewer::projectOnGeometryWithTags(const QPointF& ssPoint, const QStringList& tags) const
{
    if(!window() || !window()->isActive())
        return QVector4D(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), 0.0f);

    bool finished = false;
    Selectable* selectable = nullptr;

    PickUsingRasterizationWorker* worker = new PickUsingRasterizationWorker(this, ssPoint, tags, selectable, finished);
    window()->scheduleRenderJob(worker, QQuickWindow::AfterSynchronizingStage);
    window()->update();

    // TODO: add a timeout
    while(!finished)
        qApp->processEvents(QEventLoop::WaitForMoreEvents);

    if(selectable)
        return QVector4D(selectable->position(), 1.0f);

    return QVector4D(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), 0.0f);
}

SelectableSofaParticle* SofaViewer::pickParticle(const QPointF& ssPoint) const
{
    return pickParticleWithTags(ssPoint, QStringList());
}

SelectableSofaParticle* SofaViewer::pickParticleWithTags(const QPointF& ssPoint, const QStringList& tags) const
{
    QVector3D nearPosition = mapToWorld(ssPoint, 0.0);
    QVector3D farPosition  = mapToWorld(ssPoint, 1.0);

    QVector3D origin = nearPosition;
    QVector3D direction = (farPosition - nearPosition).normalized();

    double distanceToRay = mySofaScene->radius() / 76.0;
    double distanceToRayGrowth = 0.001;

    return mySofaScene->pickParticle(origin, direction, distanceToRay, distanceToRayGrowth, tags, roots());
}

sofa::core::visual::VisualParams* SofaViewer::setupVisualParams(sofa::core::visual::VisualParams* visualParams) const
{
    if(!visualParams)
        return nullptr;

    GLint _viewport[4];
    GLdouble _mvmatrix[16], _projmatrix[16];

    glGetIntegerv(GL_VIEWPORT, _viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, _mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, _projmatrix);

    visualParams->viewport() = sofa::helper::fixed_array<int, 4>(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
    visualParams->sceneBBox() = mySofaScene->mySofaRootNode->f_bbox.getValue();
    visualParams->setProjectionMatrix(_projmatrix);
    visualParams->setModelViewMatrix(_mvmatrix);

    return visualParams ;
}

void SofaViewer::drawManipulator(const SofaViewer& viewer) const
{
    for(Manipulator* manipulator : mySofaScene->myManipulators)
        if(manipulator)
            manipulator->draw(viewer);
}

void SofaViewer::drawEditorView(const QList<SofaComponent*>& roots,
                                bool doDrawSelected, bool doDrawManipulators) const
{
    if(!mySofaScene->isReady())
        return;

    QList<sofa::simulation::Node*> nodes;
    nodes.reserve(roots.size());
    for(SofaComponent* sofaComponent : roots)
        if(sofaComponent)
        {
            sofa::core::objectmodel::Base* base = sofaComponent->base();
            if(base)
            {
                Node* node = down_cast<Node>(base->toBaseNode());
                if(!node->visualLoop)
                {
                    msg_warning("SofaQtQuickGUI")  << "SofaViewer: The node \"" << node->getPathName() << "\" has been selected for visualization but will be skipped because it contains no VisualManagerLoop";
                    continue;
                }

                nodes.append(node);
            }
        }

    if(nodes.isEmpty() && roots.isEmpty())
        nodes.append(mySofaScene->mySofaRootNode.get());

    setupVisualParams(this->getVisualParams());

    mySofaScene->prepareSceneForDrawing();

    for(Node* node : nodes)
    {
        if(!node)
            continue;

        mySofaScene->mySofaSimulation->draw(this->getVisualParams(), node);
    }

    if(doDrawSelected)
        drawSelectedComponents(this->getVisualParams()) ;

    if(doDrawManipulators)
        drawManipulator(*this) ;
}



void SofaViewer::clearBuffers(const QSize& size, const QColor& color, const QImage& image) const
{
    // final image will be blended using premultiplied alpha
    glClearColor(color.redF() * color.alphaF(), color.greenF() * color.alphaF(), color.blueF() * color.alphaF(), color.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!image.isNull())
    {
        QOpenGLPaintDevice device(size);
        QPainter painter(&device);
        painter.drawImage(size.width() - image.width(), size.height() - image.height(), image);
    }
}

void SofaViewer::setupCamera(int width, int height, const SofaViewer& viewer) const
{
    Camera* camera = viewer.camera() ;

    // qt does not release its shader program and we do not use one so we have to release the current bound program
    glUseProgram(0);

    camera->setAspectRatio(width / (double) height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(camera->projection().constData());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(camera->view().constData());

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
}

void SofaViewer::drawSelectedComponents(sofa::core::visual::VisualParams* visualParams) const
{
    if( !visualParams )
        return ;

    sofa::core::objectmodel::Base* selectedBase = mySofaScene->mySelectedComponent->base();
    if(selectedBase)
    {
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-0.2f, 0.0f);

        myHighlightShaderProgram->bind();

        /// Draw the objects 'helpers' according to its type, the underlying idea is that we
        /// want to be able to draw information about the scene when we are editting but we don't
        /// want to hardcode this display into the components as this may depend on the application.
        ObjectRenderer::drawBaseObject(selectedBase, visualParams, true);

        myHighlightShaderProgram->release();

        glDisable(GL_POLYGON_OFFSET_LINE);

        glDepthFunc(GL_LESS);
    }
    glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
}

static QVector4D packPickingIndex(int i)
{
    return QVector4D((i & 0x000000FF) / 255.0, ((i & 0x0000FF00) >> 8) / 255.0, ((i & 0x00FF0000) >> 16) / 255.0, 0.0);
}

static int unpackPickingIndex(const std::array<unsigned char, 4>& i)
{
    return (i[0] | (i[1] << 8) | (i[2] << 16));
}


Selectable* SofaViewer::pickObject(const QPointF& ssPoint, const QStringList& tags, const QList<SofaComponent*>& roots)
{
    Selectable* selectable = nullptr;

    QList<sofa::simulation::Node*> nodes;
    nodes.reserve(roots.size());
    for(SofaComponent* sofaComponent : roots)
        if(sofaComponent)
        {
            sofa::core::objectmodel::Base* base = sofaComponent->base();
            if(base)
                nodes.append(down_cast<Node>(base->toBaseNode()));
        }

    if(nodes.isEmpty() && roots.isEmpty())
        nodes.append(mySofaScene->mySofaRootNode.get());

    QSize theNativeSize = nativeSize();
    if(!myPickingFBO || theNativeSize != myPickingFBO->size())
    {
        delete myPickingFBO;
        myPickingFBO = new QOpenGLFramebufferObject(theNativeSize, QOpenGLFramebufferObject::CombinedDepthStencil);
    }

    myPickingFBO->bind();
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);

        sofa::core::visual::VisualParams* visualParams = sofa::core::visual::VisualParams::defaultInstance();
        QVector<VisualModel*> visualModels;
        QVector<TriangleModel*> triangleModels;

        // write object index

        int index = 1;

        myPickingShaderProgram->bind();
        {
            int indexLocation = myPickingShaderProgram->uniformLocation("index");

            if(nodes.isEmpty())
                nodes.append(mySofaScene->sofaRootNode().get());

            // visual models
            for(sofa::simulation::Node* root : nodes)
            {
                if(!root)
                    continue;

                sofa::helper::vector<VisualModel*> currentVisualModels;
                root->getTreeObjects<VisualModel>(&currentVisualModels);

                if(currentVisualModels.empty())
                    continue;

                for(VisualModel* visualModel : currentVisualModels)
                {
                    if(SofaComponent::hasTag(visualModel, tags))
                    {
                        myPickingShaderProgram->setUniformValue(indexLocation, packPickingIndex(index));
                        visualModel->drawVisual(visualParams);

                        visualModels.append(visualModel);
                        index++;
                    }
                }
            }

            // triangle models

            for(sofa::simulation::Node* root : nodes)
            {
                if(!root)
                    continue;

                sofa::helper::vector<TriangleModel*> currentTriangleModels;
                root->getTreeObjects<TriangleModel>(&currentTriangleModels);

                if(currentTriangleModels.empty())
                    continue;

                for(TriangleModel* triangleModel : currentTriangleModels)
                {
                    if(SofaComponent::hasTag(triangleModel, tags))
                    {
                        sofa::component::visualmodel::VisualStyle* visualStyle = nullptr;
                        triangleModel->getContext()->get(visualStyle);

                        myPickingShaderProgram->setUniformValue(indexLocation, packPickingIndex(index));

                        if(visualStyle)
                            visualStyle->fwdDraw(visualParams);

                        triangleModel->draw(visualParams);

                        if(visualStyle)
                            visualStyle->bwdDraw(visualParams);

                        triangleModels.append(triangleModel);
                        index++;
                    }
                }
            }

            if(drawManipulators() && (tags.isEmpty() || tags.contains("manipulator", Qt::CaseInsensitive)))
                for(Manipulator* manipulator : mySofaScene->myManipulators)
                {
                    if(manipulator)
                    {
                        myPickingShaderProgram->setUniformValue(indexLocation, packPickingIndex(index));
                        manipulator->pick(*this);
                    }

                    index++;
                }
        }
        myPickingShaderProgram->release();

        // read object index
        QPointF nativePoint = mapToNative(ssPoint);
        std::array<unsigned char, 4> indexComponents;
        glReadPixels(nativePoint.x(), nativePoint.y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, indexComponents.data());

        index = unpackPickingIndex(indexComponents) - 1;

        if(-1 != index)
        {
            if(index < visualModels.size())
            {
                selectable = new SelectableSofaComponent(SofaComponent(mySofaScene, visualModels[index]));
            }
            else
            {
                index -= visualModels.size();

                if(index < triangleModels.size())
                {
                    selectable = new SelectableSofaComponent(SofaComponent(mySofaScene, triangleModels[index]));
                }
                else
                {
                    index -= triangleModels.size();

                    if(drawManipulators())
                        if((int) index < mySofaScene->myManipulators.size())
                            selectable = new SelectableManipulator(*(mySofaScene->myManipulators[index]));
                }
            }
        }

        if(selectable)
        {
            float z = 1.0;
            glReadPixels(nativePoint.x(), nativePoint.y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
            selectable->setPosition(mapToWorld(ssPoint, z));
        }
        else if(-1 != index)
        {
            msg_warning("SofaQtQuickGUI") << "SofaScene::pickObject(...) return an incorrect object index";
        }
    }
    myPickingFBO->release();

    return selectable;
}

Selectable* SofaViewer::pickObject(const QPointF& ssPoint)
{
    return pickObjectWithTags(ssPoint, QStringList());
}

Selectable* SofaViewer::pickObjectWithTags(const QPointF& ssPoint, const QStringList& tags)
{
    Selectable* selectable = nullptr;

    if(!window() || !window()->isActive())
        return selectable;

    bool finished = false;

    PickUsingRasterizationWorker* worker = new PickUsingRasterizationWorker(this, ssPoint, tags, selectable, finished);
    window()->scheduleRenderJob(worker, QQuickWindow::AfterSynchronizingStage);
    window()->update();

    // TODO: add a timeout
    while(!finished)
        qApp->processEvents(QEventLoop::WaitForMoreEvents);

    return selectable;
}

QPair<QVector3D, QVector3D> SofaViewer::boundingBox() const
{
    QVector3D min, max;
    mySofaScene->computeBoundingBox(min, max);

    return QPair<QVector3D, QVector3D>(min, max);
}

QPair<QVector3D, QVector3D> SofaViewer::rootsBoundingBox() const
{
	QVector3D min, max;
	mySofaScene->computeBoundingBox(min, max, myRoots); /*attention, ne prend en compte que le dernier �l�ment mis dans la liste ?*/

	return QPair<QVector3D, QVector3D>(min, max);
}

QVector3D SofaViewer::boundingBoxMin() const
{
    QVector3D min, max;
    mySofaScene->computeBoundingBox(min, max);

    return min;
}

QVector3D SofaViewer::boundingBoxMax() const
{
    QVector3D min, max;
    mySofaScene->computeBoundingBox(min, max);

    return max;
}

void SofaViewer::handleBackgroundImageSourceChanged(QUrl newBackgroundImageSource)
{
    QString path = newBackgroundImageSource.toEncoded();
    if(path.isEmpty())
        path = newBackgroundImageSource.toLocalFile();

    // WTF?
    path = path.replace("file://", "");
    path = path.replace("qrc:", ":");

    myBackgroundImage = QImage(path);

}

void SofaViewer::saveScreenshot(const QString& path)
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
    ScreenshotWorker(const SofaViewer* viewer, const QString& path, int width, int height) :
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
    const SofaViewer*	myViewer;
    QString				myPath;
    int					myWidth;
    int					myHeight;
};


void SofaViewer::saveScreenshotWithResolution(const QString& path, int width, int height)
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

void SofaViewer::saveCameraToFile(int uiId) const
{
    //if (!this->camera)
    //    return;

    QUrl source = mySofaScene->source();
    QString finalFilename = source.path();
    if (source.isLocalFile())
        finalFilename = source.toLocalFile();
    QString viewBasename = source.fileName() + ".qtquickview";
    QString viewPath = source.path();
    QString viewFilename = viewPath + ".qtquickview";

    QFileInfo fileInfo(viewFilename);
    QSettings viewSettings(viewFilename, QSettings::IniFormat);
    QString uiIdKey = QString(uiId) + "/";

    QVariant eyeVariant, targetVariant, upVariant;
    eyeVariant.setValue(this->camera()->eye());
    targetVariant.setValue(this->camera()->target());
    upVariant.setValue(this->camera()->up());

    viewSettings.setValue(uiIdKey + "eye", eyeVariant);
    viewSettings.setValue(uiIdKey + "target", targetVariant);
    viewSettings.setValue(uiIdKey + "up", upVariant);
    viewSettings.setValue(uiIdKey + "zFar", this->camera()->zFar());
    viewSettings.setValue(uiIdKey + "zNear", this->camera()->zNear());
    viewSettings.setValue(uiIdKey + "orthographic", this->camera()->orthographic());


    //file update/creation is done when QSettings is destroyed apparently
}

void SofaViewer::loadCameraFromFile(int uiId)
{
    QUrl source = mySofaScene->source();
    QString finalFilename = source.path();
    if (source.isLocalFile())
        finalFilename = source.toLocalFile();
    QString viewBasename = source.fileName() + ".qtquickview";
    QString viewPath = source.path();
    QString viewFilename = viewPath + ".qtquickview";

    QFileInfo fileInfo(viewFilename);

    if (!fileInfo.exists())
        return; //TODO: inform the user somehow

    QSettings viewSettings(viewFilename, QSettings::IniFormat);
    QString uiIdKey = QString(uiId) + "/";

    double zFar = viewSettings.value(uiIdKey + "zFar").toDouble();
    double zNear = viewSettings.value(uiIdKey + "zNear").toDouble();
    bool orthographic = viewSettings.value(uiIdKey + "orthographic").toBool();
    QVariant eyeVariant = viewSettings.value(uiIdKey + "eye");
    QVariant targetVariant = viewSettings.value(uiIdKey + "target");
    QVariant upVariant = viewSettings.value(uiIdKey + "up");
    QVector3D eye = eyeVariant.value<QVector3D>();
    QVector3D target = targetVariant.value<QVector3D>();
    QVector3D up = upVariant.value<QVector3D>();

    this->camera()->lookAt(eye, target, up);
    this->camera()->setZNear(zNear);
    this->camera()->setZFar(zFar);
    this->camera()->setOrthographic(orthographic);

}

QSize SofaViewer::nativeSize() const
{
    return nativeRect().size();
}

QRect SofaViewer::nativeRect() const
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

QRect SofaViewer::qtRect() const
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

QPointF SofaViewer::mapToNative(const QPointF& ssPoint) const
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

void SofaViewer::viewAll(float radiusFactor)
{
    if(!myCamera || !mySofaScene || !mySofaScene->isReady())
        return;

    QVector3D min, max;
    mySofaScene->computeBoundingBox(min, max, myRoots);

    myCamera->fit(min, max, 1.5f * radiusFactor);
}

QSGNode* SofaViewer::updatePaintNode(QSGNode* inOutNode, UpdatePaintNodeData* inOutData)
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

void SofaViewer::internalRender(int width, int height) const
{
    QSize size(width, height);
    if(size.isEmpty())
        return;

    clearBuffers(size, myBackgroundColor, myBackgroundImage);

    if(!myCamera)
        return;

    glEnable(GL_LIGHTING);
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

        GLint _viewport[4];
        GLdouble _mvmatrix[16], _projmatrix[16];

        glGetIntegerv (GL_VIEWPORT, _viewport);
        glGetDoublev  (GL_MODELVIEW_MATRIX, _mvmatrix);
        glGetDoublev  (GL_PROJECTION_MATRIX, _projmatrix);

        m_visualParams->viewport() = sofa::helper::fixed_array<int, 4>(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
        m_visualParams->sceneBBox() = mySofaScene->sofaRootNode()->f_bbox.getValue();
        m_visualParams->setProjectionMatrix(_projmatrix);
        m_visualParams->setModelViewMatrix(_mvmatrix);
    }

    // draw the scene frame
    if(myDrawFrame)
        renderFrame();

    preDraw();
    drawEditorView(roots(),false,false);
    postDraw();


    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void SofaViewer::renderFrame() const
{
    float size = 1.0f;
    if(mySofaScene)
        size = mySofaScene->radius() * 0.1;

    if(0.0f == size)
        size = 1.0f;

    m_visualParams->drawTool()->drawFrame(sofa::defaulttype::Vector3(), sofa::defaulttype::Quaternion(), sofa::defaulttype::Vector3(size, size, size));
}


SofaViewer::SofaRenderer::SofaRenderer(SofaViewer* viewer) : QQuickFramebufferObject::Renderer(),
    myViewer(viewer),
    myAntialiasingSamples(0)
{
}

QOpenGLFramebufferObject* SofaViewer::SofaRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(myAntialiasingSamples);

    myViewer->myFBO = new QOpenGLFramebufferObject(size, format);
    return myViewer->myFBO;
}

void SofaViewer::SofaRenderer::synchronize(QQuickFramebufferObject* quickFramebufferObject)
{
    SofaViewer* viewer = qobject_cast<SofaViewer*>(quickFramebufferObject);
    if(!viewer)
        return;

    if(myAntialiasingSamples != viewer->myAntialiasingSamples)
    {
        myAntialiasingSamples = viewer->myAntialiasingSamples;
        invalidateFramebufferObject();
    }
}

void SofaViewer::SofaRenderer::render()
{
    if(!myViewer)
    {
        update();

        return;
    }

    if(myViewer->autoPaint())
    {
        update();

        if(!myViewer->isVisible() && !myViewer->alwaysDraw())
            return;
    }

    myViewer->internalRender(myViewer->width(), myViewer->height());
}

}

}

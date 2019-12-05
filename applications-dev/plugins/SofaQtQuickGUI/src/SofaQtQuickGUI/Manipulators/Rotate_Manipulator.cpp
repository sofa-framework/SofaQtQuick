#include "Rotate_Manipulator.h"

#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <qmath.h>

using namespace sofa::defaulttype;
namespace sofaqtquick
{

Rotate_Manipulator::Rotate_Manipulator(QObject* parent)
    : Manipulator(parent)
{
    m_name = "Rotate_Manipulator";
    m_index = -1;
}

void Rotate_Manipulator::drawXAxis(const Vec3d& pos)
{
    Quatd orientation(0, 1, 0, 90);
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    glTranslated(pos.x(), pos.y(), pos.z());
    drawtools.drawDisk(radius, resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightred : red);
}

void Rotate_Manipulator::drawYAxis(const Vec3d& pos)
{
    Quatd orientation(1, 0, 0, 90);
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    glTranslated(pos.x(), pos.y(), pos.z());
    drawtools.drawDisk(radius, resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightgreen : green);
}

void Rotate_Manipulator::drawZAxis(const Vec3d& pos)
{
    Quatd orientation(1, 0, 0, 0);
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    glTranslated(pos.x(), pos.y(), pos.z());
    drawtools.drawDisk(radius, resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightblue : blue);
}

void Rotate_Manipulator::drawCamAxis(const Vec3d& pos)
{
    QQuaternion o = cam->orientation();
    Quaternion orientation = Quaternion(o.x(), o.y(), -o.z(), -o.scalar() * 360.0);
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    glTranslated(pos.x(), pos.y(), pos.z());
    drawtools.drawDisk(radius * 1.2f, resolution, lightwhite);
    drawtools.drawCircle(radius * 1.2f, lineThickness, resolution, m_index == 3 ? highlightwhite : white);
}

sofa::Data<Vec3d>* getData()
{
    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase()) return nullptr;

    sofa::Data<Vec3d>* data = nullptr;
    for (auto& d : obj->rawBase()->getDataFields())
        if (d->getValueTypeString() == "Vec3d" && d->getName() == "rotation")
        {
            data = dynamic_cast<sofa::Data<Vec3d>*>(d);
            break;
        }
    if (!data) return nullptr;
    return data;
}

void Rotate_Manipulator::internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking)
{
    cam = viewer.camera();
    if (!cam) return;

    sofa::Data<Vec3d>* data = getData();
    if (!data) return;

    Vec3d pos = data->getValue();

    float distanceToPoint = viewer.projectOnPlane(QPointF(viewer.width(), viewer.height()),
                                                  QVector3D(float(pos.x()), float(pos.y()), float(pos.z())),
                                                  cam->direction()).distanceToPoint(cam->eye());

    radius = 0.1f * distanceToPoint;
    lineThickness = 2.0f;
    resolution = 64;


    highlightred = Vec4f(1.0f, 0.5f, 0.5f, 1.0f);
    highlightgreen = Vec4f(0.5f, 1.0f, 0.5f, 1.0f);
    highlightblue = Vec4f(0.5f, 0.5f, 1.0f, 1.0f);
    highlightwhite = Vec4f(1.0f, 1.0f, 1.0f, isPicking ? 1.0f : 0.2f);

    lightred = Vec4f(0.86f, 0.27f, 0.33f, isPicking ? 1.0f : 0.2f);
    lightgreen = Vec4f(0.56f, 0.79f, 0.0f, isPicking ? 1.0f : 0.2f);
    lightblue = Vec4f(0.30f, 0.53f, 0.94f, isPicking ? 1.0f : 0.2f);
    lightwhite = Vec4f(1.0f, 1.0f, 1.0f, isPicking ? 1.0f : 0.2f);

    red = Vec4f(0.86f, 0.27f, 0.33f, 1.0f);
    green = Vec4f(0.56f, 0.79f, 0.0f, 1.0f);
    blue = Vec4f(0.30f, 0.53f, 0.94f, 1.0f);
    white = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
    black = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
    yellow = Vec4f(0.957f, 0.65f, 0.0f, 1.0f);


    /// Doing the actual drawing...:
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glEnable(GL_MULTISAMPLE_ARB);
    glDisable(GL_DEPTH_TEST);

    if (pickIndex == -1 || pickIndex == 0)
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        drawXAxis(pos);
        glPopMatrix();
    }

    if (pickIndex == -1 || pickIndex == 1)
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        drawYAxis(pos);
        glPopMatrix();
    }

    if (pickIndex == -1 || pickIndex == 2)
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        drawZAxis(pos);
        glPopMatrix();
    }

    if (pickIndex == -1 || pickIndex == 3)
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        drawCamAxis(pos);
        glPopMatrix();
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE_ARB);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Rotate_Manipulator::manipulate(const QPointF& mouse, SofaViewer* viewer) const
{
    Camera* cam = viewer->camera();
    if (!cam) return;

    sofa::Data<Vec3d>* data = getData();
    if (!data) return;
    Vec3d pos = data->getValue();


    QVector3D rotated;
    switch (m_index)
    {
    case 0: // only move along X axis
        rotated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                           float(pos.y()),
                                                           float(pos.z())),
                                          QVector3D(1,0,0));
        break;
    case 1: // only move along Y axis
        rotated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                           float(pos.y()),
                                                           float(pos.z())),
                                          QVector3D(0,1,0));
        break;
    case 2: // only move along Z axis
        rotated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                           float(pos.y()),
                                                           float(pos.z())),
                                          QVector3D(0,0,1));
        break;
    case 3: // only move along XY plane
        rotated = viewer->projectOnPlane(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(0,0,1));
        break;
    };

    data->setValue(Vec3d(double(rotated.x()), double(rotated.y()), double(rotated.z())));
}

int Rotate_Manipulator::getIndices() const
{
    return 4;
}

void Rotate_Manipulator::setIndex(int index)
{
    m_index = index;
}

}  // namespace sofaqtquick

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
    if (m_index == 0)
        drawtools.drawDisk(radius, _from, _to, resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightred : red);
}

void Rotate_Manipulator::drawYAxis(const Vec3d& pos)
{
    Quatd orientation(1, 0, 0, 90);
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    glTranslated(pos.x(), pos.y(), pos.z());
    if (m_index == 1)
        drawtools.drawDisk(radius, _from, _to, resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightgreen : green);
}

void Rotate_Manipulator::drawZAxis(const Vec3d& pos)
{
    Quatd orientation(1, 0, 0, 0);
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    glTranslated(pos.x(), pos.y(), pos.z());
    if (m_index == 2)
        drawtools.drawDisk(radius, _from, _to, resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightblue : blue);
}

void Rotate_Manipulator::drawCamAxis(const Vec3d& pos)
{
    QQuaternion o = cam->orientation();
    Quaternion orientation = Quaternion(o.x(), o.y(), o.z(), o.scalar()) * Quaternion(1,0,0, M_PI);
    glRotated(orientation[3] * 180.0 / M_PI, orientation[0], orientation[1], orientation[2]);
    glTranslated(pos.x(), pos.y(), pos.z());
    if (m_index == 3)
        drawtools.drawDisk(radius * 1.2f, _from, _to, resolution, lightwhite);
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

double getAngle(const QVector3D& mouse, SofaViewer* viewer, const QVector3D& center, const QVector3D& right, const QVector3D& up)
{
    QVector3D dir = (mouse - center).normalized();

    double angle = std::acos(QVector3D::dotProduct(up, dir));
    if(QVector3D::dotProduct(right, dir) < 0.0)
        angle = -angle;
    return angle;
}

void Rotate_Manipulator::mouseMoved(const QPointF& mouse, SofaViewer* viewer)
{
    cam = viewer->camera();
    if (!cam) return;

    sofa::Data<Vec3d>* data = getData();
    if (!data) return;
    Vec3d pos = data->getValue();
    QVector3D center(pos.x(), pos.y(), pos.z());

    QVector3D X(1,0,0);
    QVector3D Y(0,1,0);
    QVector3D Z(0,0,1);
    QVector3D m;
    switch (m_index)
    {
    case 0:
        m = viewer->projectOnPlane(mouse, center, X);
        setMark(_startAngle, getAngle(m, viewer, center, X, Y));
        break;
    case 1:
        m = viewer->projectOnPlane(mouse, center, Y);
        setMark(_startAngle, getAngle(m, viewer, center, Y, Z));
        break;
    case 2:
        m = viewer->projectOnPlane(mouse, center, Z);
        setMark(_startAngle, getAngle(m, viewer, center, Z, X));
        break;
    case 3:
        cam = viewer->camera();
        if (!cam) return;
        double z = viewer->computeDepth(QVector3D(pos.x(), pos.y(), pos.z()));
        m = viewer->mapToWorld(mouse, z);
        setMark(_startAngle, getAngle(m, viewer, center, cam->right(), cam->up()));
        break;
    };

    std::cout << _from << " -> " << _to << std::endl;
}


void Rotate_Manipulator::mousePressed(const QPointF& mouse, SofaViewer* viewer)
{
    sofa::Data<Vec3d>* data = getData();
    if (!data) return;
    Vec3d pos = data->getValue();
    QVector3D center(pos.x(), pos.y(), pos.z());

    QVector3D X(1,0,0);
    QVector3D Y(0,1,0);
    QVector3D Z(0,0,1);
    QVector3D m;
    switch (m_index)
    {
    case 0:
        m = viewer->projectOnPlane(mouse, center, X);
        _startAngle = getAngle(m, viewer, center, X, Y);
        break;
    case 1:
        m = viewer->projectOnPlane(mouse, center, Y);
        _startAngle = getAngle(m, viewer, center, Y, Z);
        break;
    case 2:
        m = viewer->projectOnPlane(mouse, center, Z);
        _startAngle = getAngle(m, viewer, center, Z, X);
        break;
    case 3:
        cam = viewer->camera();
        if (!cam) return;
        double z = viewer->computeDepth(QVector3D(pos.x(), pos.y(), pos.z()));
        m = viewer->mapToWorld(mouse, z);
        _startAngle = getAngle(m, viewer, center, cam->right(), cam->up());
        break;
    };
}

void Rotate_Manipulator::mouseReleased(const QPointF& mouse, SofaViewer* viewer)
{

}


int Rotate_Manipulator::getIndices() const
{
    return 4;
}

void Rotate_Manipulator::setMark(double from, double to)
{
    _from = from;
    _to = to;

    if(_from > _to)
    {
        _from = fmod(_from, 2.0 * M_PI);
        _to = to + 2.0 * M_PI;
    }

    drawMark = true;
}

void Rotate_Manipulator::unsetMark()
{
    drawMark = false;
    _from = 0;
    _to = 0;

}

}  // namespace sofaqtquick

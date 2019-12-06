#include "Rotate_Manipulator.h"
#include "Translate_Manipulator.h"

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
    glTranslated(pos.x(), pos.y(), pos.z());
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    if (m_index == 0)
        drawtools.drawDisk(radius, _from, _to, resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightred : red);
    glRotated(-orientation[3], orientation[0], orientation[1], orientation[2]);
    glTranslated(-pos.x(), -pos.y(), -pos.z());
}

void Rotate_Manipulator::drawYAxis(const Vec3d& pos)
{
    Quatd orientation(1, 0, 0, -90);
    glTranslated(pos.x(), pos.y(), pos.z());
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    if (m_index == 1)
        drawtools.drawDisk(radius, _from, _to, resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightgreen : green);
    glRotated(-orientation[3], orientation[0], orientation[1], orientation[2]);
    glTranslated(-pos.x(), -pos.y(), -pos.z());
}

void Rotate_Manipulator::drawZAxis(const Vec3d& pos)
{
    Quatd orientation(1, 0, 0, 0);
    glTranslated(pos.x(), pos.y(), pos.z());
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    if (m_index == 2)
        drawtools.drawDisk(radius, _from, _to, resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightblue : blue);
    glRotated(-orientation[3], orientation[0], orientation[1], orientation[2]);
    glTranslated(-pos.x(), -pos.y(), -pos.z());
}

void Rotate_Manipulator::drawCamAxis(const Vec3d& pos)
{
    QQuaternion o = cam->orientation();
    Quaternion orientation = Quaternion(o.x(), o.y(), o.z(), o.scalar()) * Quaternion(1,0,0, M_PI);
    glRotated(orientation[3] * 180.0 / M_PI, orientation[0], orientation[1], orientation[2]);
    if (m_index == 3)
        drawtools.drawDisk(radius * 1.2f, _from, _to, resolution, lightwhite);
    drawtools.drawCircle(radius * 1.2f, lineThickness, resolution, m_index == 3 ? highlightwhite : white);
}

sofa::Data<Vec3d>* Rotate_Manipulator::getData()
{
    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase()) return nullptr;
    /// @bmarques TODO: We need a way to select a default data field to manipulate
    /// Then we'll also need a way to manually pick which datafield we want to manipulate
    for (auto& d : obj->rawBase()->getDataFields())
        if (d->getValueTypeString() == "Vec3d" && (d->getName() == "rotation" || d->getName() == "orientation"))
            return dynamic_cast<sofa::Data<Vec3d>*>(d);
    return nullptr;
}

void drawDottedLine(const Vec3d& p1, const Vec3d& p2, int factor, ushort stipple, float lineThickness, const Vec4f& color)
{
    glPushAttrib(GL_ENABLE_BIT);

    glLineStipple(factor, stipple);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glColor4f(color[0], color[1], color[2], color[3]);
    glVertex3f(p1[0], p1[1], p1[2]);
    glVertex3f(p2[0], p2[1], p2[2]);
    glEnd();

    glPopAttrib();
}

void Rotate_Manipulator::internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking)
{
    cam = viewer.camera();
    if (!cam) return;

    sofa::Data<Vec3d>* posData = Translate_Manipulator::getData();
    if (!posData) return;

    Vec3d pos = posData->getValue();
    QVector3D center(pos.x(), pos.y(), pos.z());
    if (m_index == -1)
        mX = mY = mZ = mCam = center;

    float distanceToPoint = viewer.projectOnPlane(QPointF(viewer.width(), viewer.height()),
                                                  QVector3D(float(pos.x()), float(pos.y()), float(pos.z())),
                                                  cam->direction()).distanceToPoint(cam->eye());

    radius = 0.1f * distanceToPoint;
    lineThickness = isPicking ? 4.0f : 2.0f;
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
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_MULTISAMPLE_ARB);
    glDisable(GL_DEPTH_TEST);

    if (!isPicking) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    if (pickIndex == -1 || pickIndex == 3)
    {
        QVector4D position = cam->projection() * cam->view() * QVector4D(center, 1.0);
        position = position / position.w();
        QVector4D themouse = cam->projection() * cam->view() * QVector4D(mCam, 1.0);
        themouse = themouse / themouse.w();
        themouse = themouse - position;
        glTranslatef(position.x(), position.y(), position.z());
        glScalef(viewer.height() / viewer.width(), 1.0f, 1.0f);
        float camradius = 0.24f;
        if (m_index == 3)
            drawtools.drawDisk(camradius * 1.2f, _from, _to, resolution, lightwhite);
        drawDottedLine(Vec3d(0,0,0),
                       Vec3d(themouse.x(), themouse.y(), themouse.z()),
                       3, 0xAAAA, lineThickness, white);

        drawtools.drawCircle(camradius * 1.2f, lineThickness, resolution, m_index == 2 ? highlightwhite : white);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    if (pickIndex == -1 || pickIndex == 0)
    {
        drawXAxis(pos);
        drawDottedLine(pos, Vec3d(mX.x(), mX.y(), mX.z()), 3, 0xAAAA, lineThickness, white);
    }

    if (pickIndex == -1 || pickIndex == 1)
    {
        drawYAxis(pos);
        drawDottedLine(pos, Vec3d(mY.x(), mY.y(), mY.z()), 3, 0xAAAA, lineThickness, white);
    }

    if (pickIndex == -1 || pickIndex == 2)
    {
        drawZAxis(pos);
        drawDottedLine(pos, Vec3d(mZ.x(), mZ.y(), mZ.z()), 3, 0xAAAA, lineThickness, white);
    }

//    if (pickIndex == -1 || pickIndex == 3)
//    {
//        drawCamAxis(pos);
//        drawDottedLine(pos, Vec3d(mCam.x(), mCam.y(), mCam.z()), 3, 0xAAAA, lineThickness, white);
//    }

    glDisable(GL_MULTISAMPLE_ARB);
    if (!isPicking) {
        glDisable(GL_BLEND);
    }


    glEnable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE_ARB);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
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

    sofa::Data<Vec3d>* posData = Translate_Manipulator::getData();
    if (!posData) return;
    Vec3d pos = posData->getValue();
    QVector3D center(pos.x(), pos.y(), pos.z());

    QVector3D X(1,0,0);
    QVector3D Y(0,1,0);
    QVector3D Z(0,0,1);
    switch (m_index)
    {
    case 0: {
        mX = viewer->projectOnPlane(mouse, center, X);
        setMark(_startAngle, getAngle(mX, viewer, center, X, Y));
        Quaternion q;
        Quaternion addedAngle(1, 0, 0, (_to - _from) / M_PI * 180.0);
        Vec3d& rot = *Rotate_Manipulator::getData()->beginEdit();
        rot = (q.fromEuler(rot.x(), rot.y(), rot.z()) * addedAngle).toEulerVector();
        break;
    }
    case 1: {
        mY = viewer->projectOnPlane(mouse, center, Y);
        setMark(_startAngle, getAngle(mY, viewer, center, Y, Z));
        Quaternion q;
        Quaternion addedAngle(0, 1, 0, (_to - _from) / M_PI * 180.0);
        Vec3d& rot = *Rotate_Manipulator::getData()->beginEdit();
        rot = (q.fromEuler(rot.x(), rot.y(), rot.z()) * addedAngle).toEulerVector();
        break;
    }
    case 2: {
        mZ = viewer->projectOnPlane(mouse, center, Z);
        setMark(_startAngle, getAngle(mZ, viewer, center, Z, X));
        Quaternion q;
        Quaternion addedAngle(0, 0, 1, (_to - _from) / M_PI * 180.0);
        Vec3d& rot = *Rotate_Manipulator::getData()->beginEdit();
        rot = (q.fromEuler(rot.x(), rot.y(), rot.z()) * addedAngle).toEulerVector();
        break;
    }
    case 3: {
        cam = viewer->camera();
        if (!cam) return;
        double z = viewer->computeDepth(QVector3D(pos.x(), pos.y(), pos.z()));
        mCam = viewer->mapToWorld(mouse, z);
        setMark(_startAngle, getAngle(mCam, viewer, center, cam->right(), cam->up()));
        Quaternion q;
        Quaternion addedAngle(cam->direction().x(),
                              cam->direction().y(),
                              cam->direction().z(),
                              (_to - _from) / M_PI * 180.0);
        Vec3d& rot = *Rotate_Manipulator::getData()->beginEdit();
        rot = (q.fromEuler(rot.x(), rot.y(), rot.z()) * addedAngle).toEulerVector();
        break;
    }};

    Rotate_Manipulator::getData()->endEdit();

    std::cout << _from << " -> " << _to << std::endl;
}


void Rotate_Manipulator::mousePressed(const QPointF& mouse, SofaViewer* viewer)
{
    sofa::Data<Vec3d>* data = Translate_Manipulator::getData();
    if (!data) return;
    Vec3d pos = data->getValue();
    QVector3D center(pos.x(), pos.y(), pos.z());

    QVector3D X(1,0,0);
    QVector3D Y(0,1,0);
    QVector3D Z(0,0,1);
    switch (m_index)
    {
    case 0:
        mX = viewer->projectOnPlane(mouse, center, X);
        _startAngle = getAngle(mX, viewer, center, X, Y);
        break;
    case 1:
        mY = viewer->projectOnPlane(mouse, center, Y);
        _startAngle = getAngle(mY, viewer, center, Y, Z);
        break;
    case 2:
        mZ = viewer->projectOnPlane(mouse, center, Z);
        _startAngle = getAngle(mZ, viewer, center, Z, X);
        break;
    case 3:
        cam = viewer->camera();
        if (!cam) return;
        double z = viewer->computeDepth(QVector3D(pos.x(), pos.y(), pos.z()));
        mCam = viewer->mapToWorld(mouse, z);
        _startAngle = getAngle(mCam, viewer, center, cam->right(), cam->up());
        break;
    };
}

void Rotate_Manipulator::mouseReleased(const QPointF& mouse, SofaViewer* viewer)
{
    sofa::Data<Vec3d>* data = Translate_Manipulator::getData();
    if (!data) return;
    Vec3d pos = data->getValue();
    QVector3D center(pos.x(), pos.y(), pos.z());
    mX = mY = mZ = mCam = center;
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

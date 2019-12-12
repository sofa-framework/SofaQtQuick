#include "Rotate_Manipulator.h"
#include "Translate_Manipulator.h"

#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Helper/sofaqtconversions.h>
#include <qmath.h>

using namespace sofa::defaulttype;
namespace sofaqtquick
{
using namespace helper;

Rotate_Manipulator::Rotate_Manipulator(QObject* parent)
    : Manipulator(parent)
{
    m_name = "Rotate_Manipulator";
    m_index = -1;
}

void Rotate_Manipulator::drawXAxis(const QVector3D& pos)
{
    QVector4D axisAngle(0, 1, 0, 90);
    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    if (m_index == 0 && drawMark)
        drawtools.drawDisk(radius, double(toRadians(_from)), double(toRadians(_to)), resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightred : red);
    glRotatef(-axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    glTranslatef(-pos.x(), -pos.y(), -pos.z());
}

void Rotate_Manipulator::drawYAxis(const QVector3D& pos)
{
    QVector4D axisAngle(1, 0, 0, -90);
    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    if (m_index == 1 && drawMark)
        drawtools.drawDisk(radius, double(toRadians(_from)), double(toRadians(_to)), resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightgreen : green);
    glRotatef(-axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    glTranslatef(-pos.x(), -pos.y(), -pos.z());
}

void Rotate_Manipulator::drawZAxis(const QVector3D& pos)
{
    QVector4D axisAngle(1, 0, 0, 0);
    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    if (m_index == 2 && drawMark)
        drawtools.drawDisk(radius, double(toRadians(_from)), double(toRadians(_to)), resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightblue : blue);
    glRotatef(-axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    glTranslatef(-pos.x(), -pos.y(), -pos.z());
}


void Rotate_Manipulator::drawCamAxis(const QVector3D& pos)
{
    QVector4D axisAngle = toAxisAngle(cam->orientation());
    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    if (m_index == 3 && drawMark)
        drawtools.drawDisk(radius * 1.2f, double(toRadians(_from)), double(toRadians(_to)), resolution, lightwhite);
    drawtools.drawCircle(radius * 1.2f, lineThickness, resolution, m_index == 2 ? highlightwhite : white);
    glRotatef(-axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    glTranslatef(-pos.x(), -pos.y(), -pos.z());
}

sofa::core::objectmodel::BaseData* Rotate_Manipulator::getData()
{
    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase()) return nullptr;
    /// @bmarques TODO: We need a way to select a default data field to manipulate
    /// Then we'll also need a way to manually pick which datafield we want to manipulate
    for (auto& d : obj->rawBase()->getDataFields()){
        if (d->getValueTypeString() == "Vec3d" && (d->getName() == "rotation" || d->getName() == "orientation" || d->getName() == "direction"))
            return d;
        else if ((d->getValueTypeString() == "Quatf" || d->getValueTypeString() == "Quatd" ||
                  d->getValueTypeString() == "Quat<double>" || d->getValueTypeString() == "Quat<float>")
                  && (d->getName() == "rotation" || d->getName() == "orientation"))
            return d;
    }
    return nullptr;
}

void drawDottedLine(const Vec3d& p1, const Vec3d& p2, int factor, ushort stipple, float lineThickness, const Vec4f& color)
{
    glPushAttrib(GL_ENABLE_BIT);

    glLineWidth(lineThickness);
    glLineStipple(factor, stipple);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glColor4f(color[0], color[1], color[2], color[3]);
    glVertex3d(p1[0], p1[1], p1[2]);
    glVertex3d(p2[0], p2[1], p2[2]);
    glEnd();

    glPopAttrib();
}

void Rotate_Manipulator::internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking)
{
    cam = viewer.camera();
    if (!cam) return;

    sofa::Data<Vec3d>* posData = dynamic_cast<sofa::Data<Vec3d>*>(Translate_Manipulator::getData());
    if (!posData) return;

    QVector3D center = toQVector3D(posData->getValue());
    if (m_index == -1)
        mX = mY = mZ = mCam = center;

    float distanceToPoint = viewer.projectOnPlane(QPointF(viewer.width()/2, viewer.height()/2),
                                                  center,
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

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glEnable(GL_MULTISAMPLE_ARB);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    if (!isPicking)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    if (pickIndex == -1 || pickIndex == 0)
    {
        drawXAxis(center);
        drawDottedLine(toVec3d(center), toVec3d(mX), 3, 0xAAAA, lineThickness, white);
    }

    if (pickIndex == -1 || pickIndex == 1)
    {
        drawYAxis(center);
        drawDottedLine(toVec3d(center), toVec3d(mY), 3, 0xAAAA, lineThickness, white);
    }

    if (pickIndex == -1 || pickIndex == 2)
    {
        drawZAxis(center);
        drawDottedLine(toVec3d(center), toVec3d(mZ), 3, 0xAAAA, lineThickness, white);
    }

    if (pickIndex == -1 || pickIndex == 3)
    {
        drawCamAxis(center);
        drawDottedLine(toVec3d(center), toVec3d(mCam), 3, 0xAAAA, lineThickness, white);
    }



    // VISUAL DEBUG: Drawing a frame
    {
        sofa::core::objectmodel::BaseData* rotData = getData();
        QVector3D rot;
        if (dynamic_cast<sofa::Data<Vec3d>*>(rotData))
        {
            rot = toQVector3D(dynamic_cast<sofa::Data<Vec3d>*>(rotData)->getValue());
        }
        else
        {
            rot = toQVector3D(dynamic_cast<sofa::Data<Quaternion>*>(rotData)->getValue().toEulerVector());
            rot = QVector3D(toDegrees(rot.x()), toDegrees(rot.y()), toDegrees(rot.z()));
        }
        QQuaternion quat = QQuaternion::fromEulerAngles(rot);

        drawtools.drawFrame(toVec3d(center), toQuaternion(quat), Vec3f(4.0f,4.0f,4.0f));
    }

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


/// Returns the angle mouse - center - up (in degrees)
float getAngle(const QVector3D& mouse, const QVector3D& center, const QVector3D& right, const QVector3D& up)
{
    QVector3D dir = (mouse - center).normalized();

    float angle = std::acos(QVector3D::dotProduct(up, dir));
    if(QVector3D::dotProduct(right, dir) < 0.0f)
        angle = float(M_PI) + (float(M_PI) - angle);
    return toDegrees(angle);
}

void Rotate_Manipulator::mouseMoved(const QPointF& mouse, SofaViewer* viewer)
{
    cam = viewer->camera();
    if (!cam) return;

    sofa::Data<Vec3d>* posData = dynamic_cast<sofa::Data<Vec3d>*>(Translate_Manipulator::getData());
    if (!posData) return;

    sofa::core::objectmodel::BaseData* rotData = getData();
    QVector3D rot = startOrientation.toEulerAngles();
    QVector3D center = toQVector3D(posData->getValue());

    QVector3D X(1,0,0);
    QVector3D Y(0,1,0);
    QVector3D Z(0,0,1);
    switch (m_index)
    {
    case 0: {
        mX = viewer->projectOnPlane(mouse, center, X);

        setMark(_startAngle, getAngle(mX, center, Y, Z) - 90.0f);
        QQuaternion q;
        QQuaternion addedAngle = QQuaternion::fromEulerAngles((_to - _from), 0, 0);
        rot = (q.fromEulerAngles(rot) * addedAngle).toEulerAngles();
        break;
    }
    case 1: {
        mY = viewer->projectOnPlane(mouse, center, Y);
        setMark(_startAngle, getAngle(mY, center, Z, X) + 90.0f);
        QQuaternion q;
        QQuaternion addedAngle = QQuaternion::fromEulerAngles(0, (_to - _from), 0);
        rot = (q.fromEulerAngles(rot) * addedAngle).toEulerAngles();
        break;
    }
    case 2: {
        mZ = viewer->projectOnPlane(mouse, center, Z);
        setMark(_startAngle, getAngle(mZ, center, X, Y));

        QQuaternion q;
        QQuaternion addedAngle = QQuaternion::fromEulerAngles(0, 0, (_to - _from));
        rot = (q.fromEulerAngles(rot) * addedAngle).toEulerAngles();
        break;
    }
    case 3: {
        cam = viewer->camera();
        if (!cam) return;
        mCam = viewer->projectOnPlane(mouse, center, cam->direction());
        setMark(_startAngle, getAngle(mCam, center, cam->right(), cam->up()));

        QQuaternion angle = QQuaternion::fromEulerAngles(0, 0, (_to - _from));
        QQuaternion qCam = angle * cam->orientation();
        qCam.normalize();
        rot = (startOrientation * qCam).toEulerAngles();
        break;
    }};

    if (dynamic_cast<sofa::Data<Vec3d>*>(rotData))
    {
        if (rotData->getName() == "direction")
        {
            QVector3D d = QQuaternion::fromEulerAngles(rot).rotatedVector(startDirection);
            dynamic_cast<sofa::Data<Vec3d>*>(rotData)->setValue(
                        toVec3d(d));
        }
        dynamic_cast<sofa::Data<Vec3d>*>(rotData)->setValue(toVec3d(rot));
    }
    else {
        dynamic_cast<sofa::Data<Quaternion>*>(rotData)->setValue(Quaternion::fromEuler(toRadians(double(rot.x())) , toRadians(double(rot.y())), toRadians(double(rot.z()))));
    }


    std::cout << "from: " << _from << " to: " << _to << ": " << (_to - _from) << "°" << std::endl;
}


void Rotate_Manipulator::mousePressed(const QPointF& mouse, SofaViewer* viewer)
{
    sofa::Data<Vec3d>* data = dynamic_cast<sofa::Data<Vec3d>*>(Translate_Manipulator::getData());
    if (!data) return;
    Vec3d pos = data->getValue();
    QVector3D center = toQVector3D(pos);


    sofa::core::objectmodel::BaseData* rotData = getData();
    QVector3D rot;
    if (dynamic_cast<sofa::Data<Vec3d>*>(rotData))
    {
        rot = toQVector3D(dynamic_cast<sofa::Data<Vec3d>*>(rotData)->getValue());
        // convert rotation to radians then to Quaternion
        startOrientation = QQuaternion::fromEulerAngles(rot);
        if (rotData->getName() == "direction")
        {
            // if Vec3 is direction, we store the direction vector & initialize the rotation to 0,0,0
            startDirection = rot;
            startOrientation = QQuaternion::fromEulerAngles(0,0,0);
        }
    }
    else
    {
        startOrientation = toQQuaternion(dynamic_cast<sofa::Data<Quaternion>*>(rotData)->getValue());
    }


    QVector3D X(1,0,0);
    QVector3D Y(0,1,0);
    QVector3D Z(0,0,1);
    switch (m_index)
    {
    case 0:
        mX = viewer->projectOnPlane(mouse, center, X);
        _startAngle = getAngle(mX, center, Y, Z) - 90.0f;
        break;
    case 1:
        mY = viewer->projectOnPlane(mouse, center, Y);
        _startAngle = getAngle(mY, center, Z, X) + 90.0f;
        break;
    case 2:
        mZ = viewer->projectOnPlane(mouse, center, Z);
        _startAngle = getAngle(mZ, center, X, Y);
        break;
    case 3:
        cam = viewer->camera();
        if (!cam) return;
        mCam = viewer->projectOnPlane(mouse, center, cam->direction());
        _startAngle = getAngle(mCam, center, cam->right(), cam->up());
        break;
    };
    setMark(_startAngle, _startAngle);
    std::cout << "START ANGLE: " << _startAngle << "°" << std::endl;

}

void Rotate_Manipulator::mouseReleased(const QPointF& /*mouse*/, SofaViewer* /*viewer*/)
{
    sofa::Data<Vec3d>* data = dynamic_cast<sofa::Data<Vec3d>*>(Translate_Manipulator::getData());
    if (!data) return;
    Vec3d pos = data->getValue();
    QVector3D center = toQVector3D(pos);
    mX = mY = mZ = mCam = center;
    unsetMark();
}


int Rotate_Manipulator::getIndices() const
{
    return 4;
}

void Rotate_Manipulator::setMark(float from, float to)
{
    _from = from;
    _to = to;

    drawMark = true;
}

void Rotate_Manipulator::unsetMark()
{
    drawMark = false;
}

}  // namespace sofaqtquick

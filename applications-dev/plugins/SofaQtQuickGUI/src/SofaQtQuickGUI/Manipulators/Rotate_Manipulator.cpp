#include "Rotate_Manipulator.h"
#include "Translate_Manipulator.h"

#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Helper/sofaqtconversions.h>
#include <sofa/core/visual/VisualParams.h>
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

void Rotate_Manipulator::drawXAxis(const QVector3D& pos, sofa::core::visual::DrawToolGL& drawtools)
{
    QVector4D axisAngle(0, 1, 0, 90);
    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    if (m_index == 1 && drawMark)
        drawtools.drawDisk(radius, double(toRadians(_from)), double(toRadians(_to)), resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 1 ? highlightred : red);
    glRotatef(-axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    glTranslatef(-pos.x(), -pos.y(), -pos.z());
}

void Rotate_Manipulator::drawYAxis(const QVector3D& pos, sofa::core::visual::DrawToolGL& drawtools)
{
    QVector4D axisAngle(1, 0, 0, -90);
    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    if (m_index == 2 && drawMark)
        drawtools.drawDisk(radius, double(toRadians(_from)), double(toRadians(_to)), resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 2 ? highlightgreen : green);
    glRotatef(-axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    glTranslatef(-pos.x(), -pos.y(), -pos.z());
}

void Rotate_Manipulator::drawZAxis(const QVector3D& pos, sofa::core::visual::DrawToolGL& drawtools)
{
    QVector4D axisAngle(1, 0, 0, 0);
    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    if (m_index == 3 && drawMark)
        drawtools.drawDisk(radius, double(toRadians(_from)), double(toRadians(_to)), resolution, lightwhite);
    drawtools.drawCircle(radius, lineThickness, resolution, m_index == 3 ? highlightblue : blue);
    glRotatef(-axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    glTranslatef(-pos.x(), -pos.y(), -pos.z());
}


void Rotate_Manipulator::drawCamAxis(const QVector3D& pos, sofa::core::visual::DrawToolGL& drawtools)
{
    QVector4D axisAngle = toAxisAngle(cam->orientation());
    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    if (m_index == 4 && drawMark)
        drawtools.drawDisk(radius * 1.2f, double(toRadians(_from)), double(toRadians(_to)), resolution, lightwhite);
    drawtools.drawCircle(radius * 1.2f, lineThickness, resolution, m_index == 4 ? highlightwhite : white);
    glRotatef(-axisAngle.w(), axisAngle.x(), axisAngle.y(), axisAngle.z());
    glTranslatef(-pos.x(), -pos.y(), -pos.z());
}

sofa::core::objectmodel::BaseData* Rotate_Manipulator::GetData()
{
    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase()) return nullptr;
    /// @bmarques TODO: We need a way to select a default data field to manipulate
    /// Then we'll also need a way to manually pick which datafield we want to manipulate
    for (auto& d : obj->rawBase()->getDataFields()){
        if (d->getValueTypeString() == "Vec3d" && ( d->getName() == "rotation" || d->getName() == "direction"))
            return d;
        else if ((d->getValueTypeString() == "Quatf" || d->getValueTypeString() == "Quatd" ||
                  d->getValueTypeString() == "Quat<double>" || d->getValueTypeString() == "Quat<float>")
                  && (d->getName() == "rotation" || d->getName() == "orientation" || d->getName() == "quaternion"))
            return d;
    }
    return nullptr;
}


bool Rotate_Manipulator::getValue(QVector3D& direction, QQuaternion& orientation) const
{
    if (!m_isEditMode) {

        sofa::core::objectmodel::BaseData* rotData = GetData();
        QVector3D rot;
        if (dynamic_cast<sofa::Data<Vec3d>*>(rotData))
        {
            rot = toQVector3D(dynamic_cast<sofa::Data<Vec3d>*>(rotData)->getValue());
            // convert rotation to radians then to Quaternion
            orientation = QQuaternion::fromEulerAngles(rot);
            if (rotData->getName() == "direction")
            {
                // if Vec3 is direction, we store the direction vector & initialize the rotation to 0,0,0
                direction = rot;
                orientation = QQuaternion::fromEulerAngles(0,0,0);
            }
        }
        else
        {
            orientation = toQQuaternion(dynamic_cast<sofa::Data<Quaternion>*>(rotData)->getValue());
        }
        return true;
    }
    if (m_particleIndex == -1) {
        return false;
    }

    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase() || !obj->rawBase()->findData("position")) {
        return false;
    }
    auto* typeinfo = obj->rawBase()->findData("position")->getValueTypeInfo();
    const void* valueptr = obj->rawBase()->findData("position")->getValueVoidPtr();
//    std::cout << "size() " << typeinfo->size() << std::endl;
//    std::cout << "size(ptr) " << typeinfo->size(valueptr) << std::endl;
//    std::cout << "BaseType()->size() " << typeinfo->BaseType()->size() << std::endl;
//    std::cout << "particleIndex " << m_particleIndex << std::endl;
    if (typeinfo->BaseType()->size() == 7) {
        orientation.setX(     float(typeinfo->getScalarValue(valueptr, size_t(m_particleIndex) * typeinfo->BaseType()->size() + 3)));
        orientation.setY(     float(typeinfo->getScalarValue(valueptr, size_t(m_particleIndex) * typeinfo->BaseType()->size() + 4)));
        orientation.setZ(     float(typeinfo->getScalarValue(valueptr, size_t(m_particleIndex) * typeinfo->BaseType()->size() + 5)));
        orientation.setScalar(float(typeinfo->getScalarValue(valueptr, size_t(m_particleIndex) * typeinfo->BaseType()->size() + 6)));
    }
    orientation = QQuaternion(1,0,0,0);
    return true;
}

void Rotate_Manipulator::setValue(const QQuaternion& value)
{
    if (!m_isEditMode) {
        sofa::core::objectmodel::BaseData* rotData = GetData();

        if (dynamic_cast<sofa::Data<Vec3d>*>(rotData))
        {
            sofa::Data<Vec3d>* data = dynamic_cast<sofa::Data<Vec3d>*>(rotData);

            if (rotData->getName() == "direction")
            {
                data->setValue(toVec3d(value.rotatedVector(startDirection)));
                data->setPersistent(true);
            }
            else
            {
                Vec3d e = toQuaternion(value).toEulerVector();
                data->setValue(Vec3d(toDegrees(e.x()), toDegrees(e.y()), toDegrees(e.z())));
                data->setPersistent(true);
            }
        }
        else
        {
            dynamic_cast<sofa::Data<Quaternion>*>(rotData)->setValue(toQuaternion(value));
            rotData->setPersistent(true);
        }
    }
    if (m_particleIndex == -1)
        return;

    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase() || !obj->rawBase()->findData("position"))
        return;

    auto* typeinfo = obj->rawBase()->findData("position")->getValueTypeInfo();
    if (typeinfo->BaseType()->size() == 7) {
        void* valueptr = obj->rawBase()->findData("position")->beginEditVoidPtr();
        //    std::cout << "size() " << typeinfo->size() << std::endl;
        //    std::cout << "size(ptr) " << typeinfo->size(valueptr) << std::endl;
        //    std::cout << "BaseType()->size() " << typeinfo->BaseType()->size() << std::endl;
        //    std::cout << "particleIndex " << m_particleIndex << std::endl;
        typeinfo->setScalarValue(valueptr, size_t(m_particleIndex) * typeinfo->BaseType()->size() + 3    , double(value.x()));
        typeinfo->setScalarValue(valueptr, size_t(m_particleIndex) * typeinfo->BaseType()->size() + 4, double(value.y()));
        typeinfo->setScalarValue(valueptr, size_t(m_particleIndex) * typeinfo->BaseType()->size() + 5, double(value.z()));
        typeinfo->setScalarValue(valueptr, size_t(m_particleIndex) * typeinfo->BaseType()->size() + 6, double(value.scalar()));
        obj->rawBase()->findData("position")->endEditVoidPtr();
        obj->rawBase()->findData("position")->setPersistent(true);
    }
}



bool Rotate_Manipulator::getLocal() { return m_isLocal; }

void Rotate_Manipulator::setLocal(bool isLocal)
{
    if (m_isLocal != isLocal)
    {
        m_isLocal = isLocal;
        emit localChanged(isLocal);
    }
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

    sofa::core::visual::DrawToolGL& dt = *dynamic_cast<sofa::core::visual::DrawToolGL*>(
                viewer.getVisualParams()->drawTool());


    QVector3D center;
    if (!Translate_Manipulator::GetValue(center, m_isEditMode, m_particleIndex)) return;
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
    highlightblue = Vec4f(0.7f, 0.7f, 1.0f, 1.0f);
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

    if (pickIndex == -1 || pickIndex == 1)
    {
        drawXAxis(center, dt);
        drawDottedLine(toVec3d(center), toVec3d(mX), 3, 0xAAAA, lineThickness, white);
    }

    if (pickIndex == -1 || pickIndex == 2)
    {
        drawYAxis(center, dt);
        drawDottedLine(toVec3d(center), toVec3d(mY), 3, 0xAAAA, lineThickness, white);
    }

    if (pickIndex == -1 || pickIndex == 3)
    {
        drawZAxis(center, dt);
        drawDottedLine(toVec3d(center), toVec3d(mZ), 3, 0xAAAA, lineThickness, white);
    }

    if (pickIndex == -1 || pickIndex == 4)
    {
        drawCamAxis(center, dt);
        drawDottedLine(toVec3d(center), toVec3d(mCam), 3, 0xAAAA, lineThickness, white);
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

    QVector3D center;
    if (!Translate_Manipulator::GetValue(center, m_isEditMode, m_particleIndex)) return;
    QQuaternion quat;

    QVector3D X(1,0,0);
    QVector3D Y(0,1,0);
    QVector3D Z(0,0,1);
    switch (m_index)
    {
    case 1: {
        mX = viewer->projectOnPlane(mouse, center, X);

        _to = getAngle(mX, center, Y, Z) - 90.0f;
        if (m_isLocal)
            quat = startOrientation * QQuaternion::fromAxisAndAngle(X, -(_to - _from));
        else
            quat = QQuaternion::fromAxisAndAngle(X, -(_to - _from)) * startOrientation;
        break;
    }
    case 2: {
        mY = viewer->projectOnPlane(mouse, center, Y);

        _to = getAngle(mY, center, Z, X) + 90.0f;
        if (m_isLocal)
            quat = startOrientation * QQuaternion::fromAxisAndAngle(Y, -(_to - _from));
        else
            quat = QQuaternion::fromAxisAndAngle(Y, -(_to - _from)) * startOrientation;
        break;
    }
    case 3: {
        mZ = viewer->projectOnPlane(mouse, center, Z);

        _to = getAngle(mZ, center, X, Y);
        if (m_isLocal)
            quat = startOrientation * QQuaternion::fromAxisAndAngle(Z, -(_to - _from));
        else
            quat = QQuaternion::fromAxisAndAngle(Z, -(_to - _from)) * startOrientation;
        break;
    }
    case 4: {
        cam = viewer->camera();
        if (!cam) return;
        mCam = viewer->projectOnPlane(mouse, center, cam->direction());

        _to = getAngle(mCam, center, cam->right(), cam->up());
        quat = QQuaternion::fromAxisAndAngle(cam->direction().normalized(), (_to - _from))  * startOrientation;
        break;
    }};


    setValue(quat);

    emit displayTextChanged(getDisplayText());
}


void Rotate_Manipulator::mousePressed(const QPointF& mouse, SofaViewer* viewer)
{
    QVector3D center;
    if (!Translate_Manipulator::GetValue(center, m_isEditMode, m_particleIndex)) return;
    if (!getValue(startDirection, startOrientation)) return;

    QVector3D X(1,0,0);
    QVector3D Y(0,1,0);
    QVector3D Z(0,0,1);
    switch (m_index)
    {
    case 1:
        mX = viewer->projectOnPlane(mouse, center, X);
        _from = getAngle(mX, center, Y, Z) - 90.0f;
        break;
    case 2:
        mY = viewer->projectOnPlane(mouse, center, Y);
        _from = getAngle(mY, center, Z, X) + 90.0f;
        break;
    case 3:
        mZ = viewer->projectOnPlane(mouse, center, Z);
        _from = getAngle(mZ, center, X, Y);
        break;
    case 4:
        cam = viewer->camera();
        if (!cam) return;
        mCam = viewer->projectOnPlane(mouse, center, cam->direction());
        _from = getAngle(mCam, center, cam->right(), cam->up());
        break;
    };
    _to = _from;
    drawMark = true;
    emit displayTextChanged(getDisplayText());
}

void Rotate_Manipulator::mouseReleased(const QPointF& /*mouse*/, SofaViewer* /*viewer*/)
{
    QVector3D center;
    if (!Translate_Manipulator::GetValue(center, m_isEditMode, m_particleIndex)) return;
    mX = mY = mZ = mCam = center;
    drawMark = false;
    emit displayTextChanged(getDisplayText());
}


int Rotate_Manipulator::getIndices() const
{
    return 5;
}


QString Rotate_Manipulator::getDisplayText() const
{
    if (drawMark)
        return QString::fromStdString(std::to_string(int(_to - _from)) + "°");
    return "";
}

}  // namespace sofaqtquick

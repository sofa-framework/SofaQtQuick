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
    drawtools.drawCircle(pos, orientation, radius, lineThickness, resolution, red);
    glRotated(-orientation[3], orientation[0], orientation[1], orientation[2]);
}

void Rotate_Manipulator::drawYAxis(const Vec3d& pos)
{
    Quatd orientation(1, 0, 0, 90);
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    drawtools.drawCircle(pos, orientation, radius, lineThickness, resolution, green);
    glRotated(-orientation[3], orientation[0], orientation[1], orientation[2]);
}

void Rotate_Manipulator::drawZAxis(const Vec3d& pos)
{
    Quatd orientation(1, 0, 0, 0);
    glRotated(orientation[3], orientation[0], orientation[1], orientation[2]);
    drawtools.drawCircle(pos, orientation, radius, lineThickness, resolution, blue);
    glRotated(-orientation[3], orientation[0], orientation[1], orientation[2]);
}

void Rotate_Manipulator::drawCamAxis(const Vec3d& pos, bool isPicking)
{
    QQuaternion o = cam->orientation();
    Quaternion q(o.x(), o.y(), o.z(), o.scalar());
//    std::cout << q << std::endl;
    drawtools.drawCircle(pos, q, radius * 1.2f, lineThickness, resolution, white);
}

void Rotate_Manipulator::internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking)
{
    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase()) return;

    /// @bmarques TODO: We need a way to select a default data field to manipulate
    /// Then we'll also need a way to manually pick which datafield we want to manipulate
    /// Currently, let's just go through all datafields of the object,
    /// and select whichever Vec3d comes first...
    sofa::Data<Vec3d>* d1 = nullptr;
    sofa::Data<Quaternion>* d2 = nullptr;
    for (auto& d : obj->rawBase()->getDataFields())
    {
        if (d->getValueTypeString() == "Vec3d" && (d->getName() == "rotation" || d->getName() == "direction"))
        {
            d1 = dynamic_cast<sofa::Data<Vec3d>*>(d);
            break;
        }
        if (d->getValueTypeString() == "Quatd" || d->getValueTypeString() == "Quat" || d->getValueTypeString() == "Quater<float>" || d->getValueTypeString() == "Quater<double>")
        {
            d2 = dynamic_cast<sofa::Data<Quaternion>*>(d);
            break;
        }
    }
    if (!d1 && !d2) return;
    Vec3d pos = d1 ? d1->getValue() : d2->getValue().toEulerVector();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glEnable(GL_MULTISAMPLE_ARB);
    glDisable(GL_DEPTH_TEST);

    glTranslated(pos.x(), pos.y(), pos.z());

    cam = viewer.camera();
    if (!cam) return;

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

    red = Vec4f(0.86f, 0.27f, 0.33f, 1.0f);
    green = Vec4f(0.56f, 0.79f, 0.0f, 1.0f);
    blue = Vec4f(0.30f, 0.53f, 0.94f, 1.0f);
    white = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
    black = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
    yellow = Vec4f(0.957f, 0.65f, 0.0f, 1.0f);

    if (pickIndex == -1 || pickIndex == 0)
        drawXAxis(pos);

    if (pickIndex == -1 || pickIndex == 1)
        drawYAxis(pos);

    if (pickIndex == -1 || pickIndex == 2)
        drawZAxis(pos);

    if (pickIndex == -1 || pickIndex == 6)
        drawCamAxis(pos, isPicking);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE_ARB);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Rotate_Manipulator::manipulate(const QPointF& mouse, SofaViewer* viewer) const
{
    Camera* cam = viewer->camera();
    if (!cam) return;

        bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
        if (!obj || !obj->rawBase()) return;

        /// @bmarques TODO: We need a way to select a default data field to manipulate
        /// Then we'll also need a way to manually pick which datafield we want to manipulate
        /// Currently, let's just go through all datafields of the object,
        /// and select whichever Vec3d comes first...
        sofa::Data<Vec3d>* data = nullptr;
        for (auto& d : obj->rawBase()->getDataFields())
            if (d->getValueTypeString() == "Vec3d")
            {
                data = dynamic_cast<sofa::Data<Vec3d>*>(d);
                break;
            }
        if (!data) return;
        Vec3d pos = data->getValue();
    QVector3D translated;
    switch (m_index)
    {
    case 0: // only move along X axis
        translated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                           float(pos.y()),
                                                           float(pos.z())),
                                          QVector3D(1,0,0));
        break;
    case 1: // only move along Y axis
        translated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                           float(pos.y()),
                                                           float(pos.z())),
                                          QVector3D(0,1,0));
        break;
    case 2: // only move along Z axis
        translated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                           float(pos.y()),
                                                           float(pos.z())),
                                          QVector3D(0,0,1));
        break;
    case 3: // only move along XY plane
        translated = viewer->projectOnPlane(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(0,0,1));
        break;
    case 4: // only move along YZ plane
        translated = viewer->projectOnPlane(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(1,0,0));
        break;
    case 5: // only move along ZX plane
        translated = viewer->projectOnPlane(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(0,1,0));
        break;
    case 6: // only move along Camera plane
        translated = viewer->projectOnPlane(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           cam->direction());
        break;
    };

    // It's easy to overflow when translating along axis that is almost
    // parallel to camera direction....
    for (int i = 0 ; i < 3 ; ++i)
        if (isnan(translated[i]) || isinf(translated[i]))
            translated[i] = pos[i];

    data->setValue(Vec3d(double(translated.x()), double(translated.y()), double(translated.z())));
}

int Rotate_Manipulator::getIndices() const
{
    return 7;
}


}  // namespace sofaqtquick

#include "TranslateParticle_Manipulator.h"

#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Helper/sofaqtconversions.h>

using namespace sofa::defaulttype;
namespace sofaqtquick
{

TranslateParticle_Manipulator::TranslateParticle_Manipulator(QObject* parent)
    : Manipulator(parent)
{
    m_name = "TranslateParticle_Manipulator";
    m_index = -1;
}

void TranslateParticle_Manipulator::drawXArrow(const Vec3d& pos)
{
    Vec3d p1 = pos + Vec3d(squareWidth, 0.0, 0.0);
    Vec3d p2 = p1 + Vec3d(arrowLength, 0.0, 0.0);

    if (m_index == 0)
    {
        glPushAttrib(GL_ENABLE_BIT);
        glLineStipple(2, 0xAAAA);
        glEnable(GL_LINE_STIPPLE);
        glLineWidth(0.2f);
        drawtools.drawInfiniteLine(pos, Vec3d(1.0, 0.0, 0.0), lightwhite);
        drawtools.drawInfiniteLine(pos, Vec3d(-1.0, 0.0, 0.0), lightwhite);
        glDisable(GL_LINE_STIPPLE);
        glPopAttrib();
    }
    drawtools.drawArrow(p1, p2, radius, float(arrowLength) / 5.0f, radius*5.0f, m_index == 0 ? highlightred : red, 16);
}

void TranslateParticle_Manipulator::drawYArrow(const Vec3d& pos)
{
    Vec3d p1 = pos + Vec3d(0.0, squareWidth, 0.0);
    Vec3d p2 = p1 + Vec3d(0.0, arrowLength, 0.0);

    if (m_index == 1)
    {
        glPushAttrib(GL_ENABLE_BIT);
        glLineStipple(2, 0xAAAA);
        glEnable(GL_LINE_STIPPLE);
        glLineWidth(0.2f);
        drawtools.drawInfiniteLine(pos, Vec3d(0.0, 1.0, 0.0), lightwhite);
        drawtools.drawInfiniteLine(pos, Vec3d(0.0, -1.0, 0.0), lightwhite);
        glDisable(GL_LINE_STIPPLE);
        glPopAttrib();
    }
    drawtools.drawArrow(p1, p2, radius, float(arrowLength) / 5.0f, radius*5.0f, m_index == 1 ? highlightgreen : green, 16);
}

void TranslateParticle_Manipulator::drawZArrow(const Vec3d& pos)
{
    Vec3d p1 = pos + Vec3d(0.0, 0.0, squareWidth);
    Vec3d p2 = p1 + Vec3d(0.0, 0.0, arrowLength);

    if (m_index == 2)
    {
        glPushAttrib(GL_ENABLE_BIT);
        glLineStipple(2, 0xAAAA);
        glEnable(GL_LINE_STIPPLE);
        glLineWidth(0.2f);
        drawtools.drawInfiniteLine(pos, Vec3d(0.0, 0.0, 1.0), lightwhite);
        drawtools.drawInfiniteLine(pos, Vec3d(0.0, 0.0, -1.0), lightwhite);
        glDisable(GL_LINE_STIPPLE);
        glPopAttrib();
    }
    drawtools.drawArrow(p1, p2, radius, float(arrowLength) / 5.0f, radius*5.0f, m_index == 2 ? highlightblue : blue, 16);
}

void TranslateParticle_Manipulator::drawXYPlane(const Vec3d& pos)
{
    if (lightblue.w() != 1.0f)
    {
        // if we removed the alpha it's for picking
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    // Draw XY plane
    Vec3d a = pos + Vec3d(squareWidth * 2.0, squareWidth * 2.0, 0.0);
    Vec3d b = a + Vec3d(squareWidth, 0.0, 0.0);
    Vec3d c = b + Vec3d(0.0, squareWidth, 0.0);
    Vec3d d = c + Vec3d(-squareWidth, 0.0, 0.0);
    drawtools.drawQuad(a,b,c,d, Vec3d(0,0,1), m_index == 3 ? highlightblue : lightblue);
    drawtools.drawLineLoop({a,b,c,d}, lineThickness, blue);
}

void TranslateParticle_Manipulator::drawYZPlane(const Vec3d& pos)
{
    if (lightred.w() != 1.0f)
    {
        // if we removed the alpha it's for picking
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Draw YZ plane
    Vec3d a = pos + Vec3d(0.0, squareWidth * 2.0, squareWidth * 2.0);
    Vec3d b = a + Vec3d(0.0, squareWidth, 0.0);
    Vec3d c = b + Vec3d(0.0, 0.0, squareWidth);
    Vec3d d = c + Vec3d(0.0, -squareWidth, 0.0);
    drawtools.drawQuad(a,b,c,d, Vec3d(1,0,0), m_index == 4 ? highlightred : lightred);
    drawtools.drawLineLoop({a,b,c,d}, lineThickness, red);
}

void TranslateParticle_Manipulator::drawZXPlane(const Vec3d& pos)
{
    if (lightgreen.w() != 1.0f)
    {
        // if we removed the alpha it's for picking
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Draw ZX plane
    Vec3d a = pos + Vec3d(squareWidth * 2.0, 0.0, squareWidth * 2.0);
    Vec3d b = a + Vec3d(0.0, 0.0, squareWidth);
    Vec3d c = b + Vec3d(squareWidth, 0.0, 0.0);
    Vec3d d = c + Vec3d(0.0, 0.0, -squareWidth);
    drawtools.drawQuad(a,b,c,d, Vec3d(0,1,0), m_index == 5 ? highlightgreen : lightgreen);
    drawtools.drawLineLoop({a,b,c,d}, lineThickness, green);
}

void TranslateParticle_Manipulator::drawCamPlane(const Vec3d& pos, bool isPicking)
{
    Vec3d up(double(cam->up().x()),
             double(cam->up().y()),
             double(cam->up().z()));
    Vec3d right(double(cam->right().x()),
             double(cam->right().y()),
             double(cam->right().z()));

    Vec3d fwd(double(cam->direction().x()),
              double(cam->direction().y()),
              double(cam->direction().z()));

    Vec3d a = pos + (up * (squareWidth / 2.0) + (right * (squareWidth / 2.0)));
    Vec3d b = a - (up * squareWidth);
    Vec3d c = b - (right * squareWidth);
    Vec3d d = c + (up * squareWidth);
    drawtools.drawLineLoop({a,b,c,d}, lineThickness, white);
    if (m_index == 6 || isPicking)
        drawtools.drawQuad(a,b,c,d, fwd, m_index == 6 ? highlightwhite : white);

    drawtools.drawSphere(pos, crossSize*1.3f, black);
    drawtools.drawSphere(pos, crossSize, yellow);
}

sofa::core::objectmodel::BaseData* TranslateParticle_Manipulator::getData()
{
    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase()) return nullptr;
    /// @bmarques TODO: We need a way to select a default data field to manipulate
    /// Then we'll also need a way to manually pick which datafield we want to manipulate
    for (auto& d : obj->rawBase()->getDataFields())
        if (d->getValueTypeString() == "Vec3d" && (d->getName() == "translation" || d->getName() == "position"))
            return d;
    return nullptr;
}

void TranslateParticle_Manipulator::internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking)
{
    data = dynamic_cast<sofa::Data<Vec3d>*>(getData());
    if (!data) return;
    QVector3D pos = helper::toQVector3D(data->getValue());

    cam = viewer.camera();
    if (!cam) return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glEnable(GL_MULTISAMPLE_ARB);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    float distanceToPoint = viewer.projectOnPlane(QPointF(viewer.width(), viewer.height()),
                                                  pos, cam->direction()).distanceToPoint(cam->eye());

    arrowLength = 0.07 * double(distanceToPoint);
    radius = 0.001f * distanceToPoint;
    squareWidth = 0.014 * double(distanceToPoint);
    lineThickness = 1.0f;
    crossSize = 0.0028f * distanceToPoint;


    highlightred = Vec4f(1.0f, 0.5f, 0.5f, 1.0f);
    highlightgreen = Vec4f(0.5f, 1.0f, 0.5f, 1.0f);
    highlightblue = Vec4f(0.5f, 0.5f, 1.0f, 1.0f);
    highlightwhite = Vec4f(1.0f, 1.0f, 1.0f, isPicking ? 1.0f : 0.2f);

    lightred = Vec4f(0.86f, 0.27f, 0.33f, isPicking ? 1.0f : 0.2f);
    lightgreen = Vec4f(0.56f, 0.79f, 0.0f, isPicking ? 1.0f : 0.2f);
    lightblue = Vec4f(0.30f, 0.53f, 0.94f, isPicking ? 1.0f : 0.2f);
    lightwhite = Vec4f(.4f, 0.4f, 0.4f, 0.01f);

    red = Vec4f(0.86f, 0.27f, 0.33f, 1.0f);
    green = Vec4f(0.56f, 0.79f, 0.0f, 1.0f);
    blue = Vec4f(0.30f, 0.53f, 0.94f, 1.0f);
    white = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
    black = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
    yellow = Vec4f(0.957f, 0.65f, 0.0f, 1.0f);

    if (pickIndex == -1 || pickIndex == 0)
        drawXArrow(helper::toVec3d(pos));

    if (pickIndex == -1 || pickIndex == 1)
        drawYArrow(helper::toVec3d(pos));

    if (pickIndex == -1 || pickIndex == 2)
        drawZArrow(helper::toVec3d(pos));

    if (pickIndex == -1 || pickIndex == 3)
        drawXYPlane(helper::toVec3d(pos));

    if (pickIndex == -1 || pickIndex == 4)
        drawYZPlane(helper::toVec3d(pos));

    if (pickIndex == -1 || pickIndex == 5)
        drawZXPlane(helper::toVec3d(pos));

    glDisable(GL_BLEND);

    if (pickIndex == -1 || pickIndex == 6)
        drawCamPlane(helper::toVec3d(pos), isPicking);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE_ARB);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void TranslateParticle_Manipulator::mouseMoved(const QPointF& mouse, SofaViewer* viewer)
{
    Camera* cam = viewer->camera();
    if (!cam) return;

        bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
        if (!obj || !obj->rawBase()) return;

        /// @bmarques TODO: We need a way to select a default data field to manipulate
        /// Then we'll also need a way to manually pick which datafield we want to manipulate
        /// Currently, let's just go through all datafields of the object,
        /// and select whichever Vec3d comes first...
        data = dynamic_cast<sofa::Data<Vec3d>*>(getData());
        if (!data) return;
        QVector3D pos = helper::toQVector3D(data->getValue());
    QVector3D translated;
    switch (m_index)
    {
    case 0: // only move along X axis
        translated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                           float(pos.y()),
                                                           float(pos.z())),
                                          QVector3D(1,0,0)) - shift;
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
                                          QVector3D(0,0,1)) - shift;
        break;
    case 3: // only move along XY plane
        translated = viewer->projectOnPlane(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(0,0,1)) - shift;
        break;
    case 4: // only move along YZ plane
        translated = viewer->projectOnPlane(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(1,0,0)) - shift;
        break;
    case 5: // only move along ZX plane
        translated = viewer->projectOnPlane(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(0,1,0)) - shift;
        break;
    case 6: // only move along Camera plane
        translated = viewer->projectOnPlane(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           cam->direction()) - shift;
        break;
    };

    // It's easy to overflow when translating along axis that is almost
    // parallel to camera direction....
    for (int i = 0 ; i < 3 ; ++i)
        if (isnan(translated[i]) || isinf(translated[i]))
            translated[i] = pos[i];

    data->setValue(Vec3d(double(translated.x()), double(translated.y()), double(translated.z())));
    emit displayTextChanged(getDisplayText());
}

void TranslateParticle_Manipulator::mousePressed(const QPointF &mouse, SofaViewer *viewer)
{
    QVector3D pos = helper::toQVector3D(dynamic_cast<sofa::Data<Vec3d>*>(getData())->getValue());
    switch (m_index)
    {
    case 0: // only move along X axis
        shift = viewer->projectOnLine(mouse, pos, QVector3D(1,0,0)) - pos;
        break;
    case 1: // only move along Y axis
        shift = viewer->projectOnLine(mouse, pos, QVector3D(0,1,0)) - pos;
        break;
    case 2: // only move along Z axis
        shift = viewer->projectOnLine(mouse, pos, QVector3D(0,0,1)) - pos;
        break;
    case 3: // only move along XY plane
        shift = viewer->projectOnPlane(mouse, pos, QVector3D(0,0,1)) - pos;
        break;
    case 4: // only move along YZ plane
        shift = viewer->projectOnPlane(mouse, pos, QVector3D(1,0,0)) - pos;
        break;
    case 5: // only move along ZX plane
        shift = viewer->projectOnPlane(mouse, pos, QVector3D(0,1,0)) - pos;
        break;
    case 6: // only move along Camera plane
        shift = viewer->projectOnPlane(mouse, pos, cam->direction()) - pos;
        break;
    };

    active = true;
    emit displayTextChanged(getDisplayText());
}

void TranslateParticle_Manipulator::mouseReleased(const QPointF &/*mouse*/, SofaViewer */*viewer*/)
{
    active = false;
    emit displayTextChanged(getDisplayText());
}

int TranslateParticle_Manipulator::getIndices() const
{
    return 7;
}

QString TranslateParticle_Manipulator::getDisplayText() const
{
    if (active)
        return QString::fromStdString(getData()->getValueString()).replace(" ", " ; ");
    return "";
}



}  // namespace sofaqtquick

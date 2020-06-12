#include "Scale_Manipulator.h"

#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Helper/sofaqtconversions.h>
#include <sofa/core/visual/VisualParams.h>

using namespace sofa::defaulttype;
namespace sofaqtquick
{

Scale_Manipulator::Scale_Manipulator(QObject* parent)
    : Manipulator(parent)
{
    m_name = "Scale_Manipulator";
    m_index = -1;
}

void Scale_Manipulator::drawFace(const std::vector<Vec3d>& q, s_color color, sofa::core::visual::DrawTool& dt, int idx, bool dir)
{
    if (color.light.w() != 1.0f)
    {
        // if we removed the alpha it's for picking
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    bool shouldDraw = m_index == idx || (m_isUniform && m_index != -1);
    glEnable(GL_DEPTH_TEST);
    dt.drawQuad(q[0],q[1],q[2],q[3], (dir ? -color.primitive : color.primitive), shouldDraw ? color.highlight : color.light);
    dt.drawLineLoop({q[0],q[1],q[2],q[3]}, lineThickness, color.color);

    Vec3d p1 = Vec3d((q[0].x() + q[2].x()) / 2, (q[0].y() + q[2].y()) / 2, (q[0].z() + q[2].z()) / 2);
    Vec3d p2 = p1 + ((dir ? color.primitive : -color.primitive) * arrowLength);

    if (shouldDraw)
    {
        glPushAttrib(GL_ENABLE_BIT);
        glLineStipple(2, 0xAAAA);
        glEnable(GL_LINE_STIPPLE);
        glLineWidth(0.2f);
        dt.drawInfiniteLine(p1, color.primitive, white.light);
        dt.drawInfiniteLine(p2, -color.primitive, white.light);
        glDisable(GL_LINE_STIPPLE);
        glPopAttrib();
    }
    dt.drawArrow(p1, p2, radius, float(arrowLength) / 5.0f, radius*5.0f, shouldDraw ? color.highlight : color.light, 16);
    Vec3d vecLen(arrowLength / 8.0, arrowLength / 8.0, arrowLength / 8.0);

    dt.drawQuad(p2 + vecLen.linearProduct(Vec3d(-1.0, -1.0, -1.0)),
                p2 + vecLen.linearProduct(Vec3d(-1.0, 1.0, -1.0)),
                p2 + vecLen.linearProduct(Vec3d(-1.0, 1.0, 1.0)),
                p2 + vecLen.linearProduct(Vec3d(-1.0, -1.0, 1.0)),
                (dir ? -color.primitive : color.primitive),
                shouldDraw ? color.highlight : color.light * 0.4);

    dt.drawQuad(p2 + vecLen.linearProduct(Vec3d(-1.0, -1.0, -1.0)),
                p2 + vecLen.linearProduct(Vec3d(-1.0, -1.0, 1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, -1.0, 1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, -1.0, -1.0)),
                (dir ? -color.primitive : color.primitive),
                shouldDraw ? color.highlight : color.light * 0.4);

    dt.drawQuad(p2 + vecLen.linearProduct(Vec3d(-1.0, -1.0, -1.0)),
                p2 + vecLen.linearProduct(Vec3d(-1.0, 1.0, -1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, 1.0, -1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, -1.0, -1.0)),
                (dir ? -color.primitive : color.primitive),
                shouldDraw ? color.highlight : color.light * 0.4);


    dt.drawQuad(p2 + vecLen.linearProduct(Vec3d(-1.0, 1.0, -1.0)),
                p2 + vecLen.linearProduct(Vec3d(-1.0, 1.0, 1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, 1.0, 1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, 1.0, -1.0)),
                (dir ? -color.primitive : color.primitive),
                shouldDraw ? color.highlight : color.light * 0.4);

    dt.drawQuad(p2 + vecLen.linearProduct(Vec3d(1.0, 1.0, 1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, 1.0, -1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, -1.0, -1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, -1.0, 1.0)),
                (dir ? -color.primitive : color.primitive),
                shouldDraw ? color.highlight : color.light * 0.4);

    dt.drawQuad(p2 + vecLen.linearProduct(Vec3d(-1.0, -1.0, 1.0)),
                p2 + vecLen.linearProduct(Vec3d(-1.0, 1.0, 1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, 1.0, 1.0)),
                p2 + vecLen.linearProduct(Vec3d(1.0, -1.0, 1.0)),
                (dir ? -color.primitive : color.primitive),
                shouldDraw ? color.highlight : color.light * 0.4);
}


sofa::core::objectmodel::BaseData* Scale_Manipulator::GetData()
{
    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase()) return nullptr;
    /// @bmarques TODO: We need a way to select a default data field to manipulate
    /// Then we'll also need a way to manually pick which datafield we want to manipulate
    for (auto& d : obj->rawBase()->getDataFields())
        if (d->getValueTypeString() == "Vec3d" && d->getName() == "scale3d")
            return d;
    return nullptr;
}

void Scale_Manipulator::internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking)
{
    BoundingBox bbox = SofaBaseApplication::GetSelectedComponent()->rawBase()->f_bbox.getValue();
    std::vector<Vec3d> front;
    front.push_back(bbox.minBBox());
    front.push_back(Vec3d(bbox.minBBox().x(), bbox.maxBBox().y(), bbox.minBBox().z()));
    front.push_back(Vec3d(bbox.maxBBox().x(), bbox.maxBBox().y(), bbox.minBBox().z()));
    front.push_back(Vec3d(bbox.maxBBox().x(), bbox.minBBox().y(), bbox.minBBox().z()));

    std::vector<Vec3d> back;
    back.push_back(bbox.maxBBox());
    back.push_back(Vec3d(bbox.minBBox().x(), bbox.maxBBox().y(), bbox.maxBBox().z()));
    back.push_back(Vec3d(bbox.minBBox().x(), bbox.minBBox().y(), bbox.maxBBox().z()));
    back.push_back(Vec3d(bbox.maxBBox().x(), bbox.minBBox().y(), bbox.maxBBox().z()));

    std::vector<Vec3d> left;
    left.push_back(bbox.minBBox());
    left.push_back(Vec3d(bbox.minBBox().x(), bbox.minBBox().y(), bbox.maxBBox().z()));
    left.push_back(Vec3d(bbox.minBBox().x(), bbox.maxBBox().y(), bbox.maxBBox().z()));
    left.push_back(Vec3d(bbox.minBBox().x(), bbox.maxBBox().y(), bbox.minBBox().z()));

    std::vector<Vec3d> right;
    right.push_back(bbox.maxBBox());
    right.push_back(Vec3d(bbox.maxBBox().x(), bbox.minBBox().y(), bbox.maxBBox().z()));
    right.push_back(Vec3d(bbox.maxBBox().x(), bbox.minBBox().y(), bbox.minBBox().z()));
    right.push_back(Vec3d(bbox.maxBBox().x(), bbox.maxBBox().y(), bbox.minBBox().z()));

    std::vector<Vec3d> top;
    top.push_back(bbox.maxBBox());
    top.push_back(Vec3d(bbox.minBBox().x(), bbox.maxBBox().y(), bbox.maxBBox().z()));
    top.push_back(Vec3d(bbox.minBBox().x(), bbox.maxBBox().y(), bbox.minBBox().z()));
    top.push_back(Vec3d(bbox.maxBBox().x(), bbox.maxBBox().y(), bbox.minBBox().z()));

    std::vector<Vec3d> bottom;
    bottom.push_back(bbox.minBBox());
    bottom.push_back(Vec3d(bbox.minBBox().x(), bbox.minBBox().y(), bbox.maxBBox().z()));
    bottom.push_back(Vec3d(bbox.maxBBox().x(), bbox.minBBox().y(), bbox.maxBBox().z()));
    bottom.push_back(Vec3d(bbox.maxBBox().x(), bbox.minBBox().y(), bbox.minBBox().z()));


    QVector3D pos = helper::toQVector3D(Vec3d((bbox.minBBox().x() + bbox.maxBBox().x())/2.0,
                                              (bbox.minBBox().y() + bbox.maxBBox().y())/2.0,
                                              (bbox.minBBox().z() + bbox.maxBBox().z())/2.0));

    sofa::core::visual::DrawToolGL& dt = *dynamic_cast<sofa::core::visual::DrawToolGL*>(
                viewer.getVisualParams()->drawTool());

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

    red.primitive =   Vec3f(1.0f, 0.0f, 0.0f);
    green.primitive = Vec3f(0.0f, 1.0f, 0.0f);
    blue.primitive =  Vec3f(0.0f, 0.0f, 1.0f);

    red.highlight = Vec4f(1.0f, 0.5f, 0.5f, 0.7f);
    green.highlight = Vec4f(0.5f, 1.0f, 0.5f, 0.7f);
    blue.highlight = Vec4f(0.5f, 0.5f, 1.0f, 0.7f);
    white.highlight = Vec4f(1.0f, 1.0f, 1.0f, isPicking ? 1.0f : 0.2f);

    red.light = Vec4f(0.86f, 0.27f, 0.33f, isPicking ? 1.0f : 0.2f);
    green.light = Vec4f(0.56f, 0.79f, 0.0f, isPicking ? 1.0f : 0.2f);
    blue.light = Vec4f(0.30f, 0.53f, 0.94f, isPicking ? 1.0f : 0.2f);
    white.light = Vec4f(.4f, 0.4f, 0.4f, 0.01f);

    red.color = Vec4f(0.86f, 0.27f, 0.33f, 1.0f);
    green.color = Vec4f(0.56f, 0.79f, 0.0f, 1.0f);
    blue.color = Vec4f(0.30f, 0.53f, 0.94f, 1.0f);
    white.color = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
    black = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
    yellow = Vec4f(0.957f, 0.65f, 0.0f, 1.0f);

    if (pickIndex == -1 || pickIndex == 0)
        drawFace(right, red, dt, 0);
    if (pickIndex == -1 || pickIndex == 3)
        drawFace(left, red, dt, 3, false);

    if (pickIndex == -1 || pickIndex == 1)
        drawFace(top, green, dt, 1);
    if (pickIndex == -1 || pickIndex == 4)
        drawFace(bottom, green, dt, 4, false);

    if (pickIndex == -1 || pickIndex == 2)
        drawFace(back, blue, dt, 2);
    if (pickIndex == -1 || pickIndex == 5)
        drawFace(front, blue, dt, 5, false);

    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE_ARB);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Scale_Manipulator::mouseMoved(const QPointF& mouse, SofaViewer* viewer)
{
    Camera* cam = viewer->camera();
    if (!cam) return;

    bindings::SofaBase* obj = SofaBaseApplication::Instance()->getSelectedComponent();
    if (!obj || !obj->rawBase()) return;

    /// @bmarques TODO: We need a way to select a default data field to manipulate
    /// Then we'll also need a way to manually pick which datafield we want to manipulate
    /// Currently, let's just go through all datafields of the object,
    /// and select whichever Vec3d comes first...
    BoundingBox bbox = SofaBaseApplication::GetSelectedComponent()->rawBase()->f_bbox.getValue();
    QVector3D pos = helper::toQVector3D(Vec3d((bbox.minBBox().x() + bbox.maxBBox().x()) / 2.0,
                                              (bbox.minBBox().y() + bbox.maxBBox().y()) / 2.0,
                                              (bbox.minBBox().z() + bbox.maxBBox().z()) / 2.0));

    data = dynamic_cast<sofa::Data<Vec3d>*>(GetData());
    QVector3D translated;
    double ratio;
    Vec3d res;



    switch (m_index)
    {
    case 0: // move along X axis
    case 3:
        translated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(1,0,0)) - shift;
        ratio = ((m_index == 3 ? -translated.x() : translated.x()) + initialAxisLength) / initialAxisLength;

        res = Vec3d(ratio, data->getValue().y(), data->getValue().z());
        break;
    case 1: // move along Y axis
    case 4:
        translated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(0,1,0)) - shift;
        ratio = ((m_index == 4 ? -translated.y() : translated.y()) + initialAxisLength) / initialAxisLength;
        res = Vec3d(data->getValue().x(), ratio, data->getValue().z());
        break;
    case 2: // move along Z axis
    case 5:
        translated = viewer->projectOnLine(mouse, QVector3D(float(pos.x()),
                                                            float(pos.y()),
                                                            float(pos.z())),
                                           QVector3D(0,0,1)) - shift;
        ratio = ((m_index == 5 ? -translated.z() : translated.z()) + initialAxisLength) / initialAxisLength;
        res = Vec3d(data->getValue().x(), data->getValue().y(), ratio);
        break;
    default: ratio = 1.0;
        break;
    };

    // It's easy to overflow when translating along axis that is almost
    // parallel to camera direction....
    for (int i = 0 ; i < 3 ; ++i)
        if (isnan(translated[i]) || isinf(translated[i])) {
            translated[i] = pos[i];
            ratio = 1.0;
        }

    if (m_isUniform)
        data->setValue(Vec3d(ratio, ratio, ratio));
    else
        data->setValue(res);
    data->setPersistent(true);
    emit displayTextChanged(getDisplayText());
}

void Scale_Manipulator::mousePressed(const QPointF &mouse, SofaViewer *viewer)
{
    BoundingBox bbox = SofaBaseApplication::GetSelectedComponent()->rawBase()->f_bbox.getValue();
    QVector3D pos = helper::toQVector3D(Vec3d((bbox.minBBox().x() + bbox.maxBBox().x()) / 2.0,
                                              (bbox.minBBox().y() + bbox.maxBBox().y()) / 2.0,
                                              (bbox.minBBox().z() + bbox.maxBBox().z()) / 2.0));
    switch (m_index)
    {
    case 0: // move along X axis
    case 3:
        shift = viewer->projectOnLine(mouse, pos, QVector3D(1,0,0));
        initialAxisLength = bbox.maxBBox().x() - bbox.minBBox().x() / 2;
        break;
    case 1: // move along Y axis
    case 4:
        shift = viewer->projectOnLine(mouse, pos, QVector3D(0,1,0));
        initialAxisLength = bbox.maxBBox().y() - bbox.minBBox().y() / 2;
        break;
    case 2: // move along Z axis
    case 5:
        shift = viewer->projectOnLine(mouse, pos, QVector3D(0,0,1));
        initialAxisLength = bbox.maxBBox().z() - bbox.minBBox().z() / 2;
        break;
    };

    active = true;
    emit displayTextChanged(getDisplayText());
}

void Scale_Manipulator::mouseReleased(const QPointF &/*mouse*/, SofaViewer */*viewer*/)
{
    active = false;
    emit displayTextChanged(getDisplayText());
    m_index = -1;
}

int Scale_Manipulator::getIndices() const
{
    return 6;
}

QString Scale_Manipulator::getDisplayText() const
{
    const Vec3d& scale (dynamic_cast<sofa::Data<Vec3d>*>(GetData())->getValue());

    std::cout << scale << std::endl;
    if (active)
    {
        if (!m_isUniform)
        {
            switch (m_index)
            {
            case 0: // move along X axis
            case 3:
                return "X: " + QString::number(scale.x());
            case 1: // move along Y axis
            case 4:
                return "Y: " + QString::number(scale.y());
            case 2: // move along Z axis
            case 5:
                return "Z: " + QString::number(scale.z());
            };
        }
        else {
            return QString::number(scale.x());
        }
    }
    return "";
}



}  // namespace sofaqtquick

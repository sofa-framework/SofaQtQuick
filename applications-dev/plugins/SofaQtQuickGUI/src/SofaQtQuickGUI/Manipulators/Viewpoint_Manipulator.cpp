#include "Viewpoint_Manipulator.h"
#include <SofaQtQuickGUI/Helper/sofaqtconversions.h>
#include <qmath.h>

namespace sofaqtquick
{

using namespace sofa::defaulttype;

Viewpoint_Manipulator::Viewpoint_Manipulator(QObject* parent)
    : Manipulator(parent)
{
    m_name = "Viewpoint_Manipulator";
    m_index = -1;
}

void Viewpoint_Manipulator::internalDraw(const SofaViewer &viewer, int pickIndex, bool isPicking)
{
    sofa::core::visual::DrawToolGL dt;
    Camera* cam = viewer.camera();
    if (!cam) return;

    cam->setOrthographic(true);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(cam->projection().constData());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(cam->view().constData());

    glEnable(GL_MULTISAMPLE_ARB);
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    double distanceToPoint = double(viewer.projectOnPlane(QPointF(viewer.width()/2, viewer.height()/2),
                                                  cam->target(), cam->direction()).distanceToPoint(cam->eye()));

    double squareWidth = distanceToPoint / (viewer.height() / 50.0);
    Vec3d a,b,c,d;



    Vec3d p = helper::toVec3d(viewer.mapToWorld(QPointF(viewer.width() - 50,50), cam->zNear()));
/*
//    if (!isPicking || pickIndex == 0)
//    {
//        if (!isPicking)
//        {
//            // if we removed the alpha it's for picking
//            glEnable(GL_BLEND);
//            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        }

//        // Draw front plane
//        a = p + Vec3d(-squareWidth/2, -squareWidth / 2, squareWidth / 2);
//        b = a + Vec3d(squareWidth, 0.0, 0.0);
//        c = b + Vec3d(0.0, squareWidth, 0.0);
//        d = c + Vec3d(-squareWidth, 0.0, 0.0);
//        dt.drawQuad(a,b,c,d, Vec3d(0,0,1), Vec4f(1.0f,1.0f,1.0f,1.0f) );
//        dt.drawLineLoop({a,b,c,d}, 0.5f, Vec4f(.0f,1.0f,1.0f,1.0));
////        glLineWidth(2.0);
////        dt.drawLine(p, p - Vec3d(0,0, -squareWidth/2), Vec4f(.0f,.0f,1.0f,1.0f));
////        glLineWidth(1.0);
////        dt.drawSphere(p - Vec3d(0,0, -squareWidth/2), squareWidth / 7.0, Vec4f(.0f,.0f,1.0f,1.0f));
//    }

//    if (!isPicking || pickIndex == 1)
//    {
//        if (!isPicking)
//        {
//            // if we removed the alpha it's for picking
//            glEnable(GL_BLEND);
//            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        }

//        // Draw back plane
//        a = p + Vec3d(-squareWidth / 2, -squareWidth / 2, -squareWidth / 2);
//        b = a + Vec3d(squareWidth, 0.0, 0.0);
//        c = b + Vec3d(0.0, squareWidth, 0.0);
//        d = c + Vec3d(-squareWidth, 0.0, 0.0);
//        dt.drawQuad(a,b,c,d, Vec3d(0,0,1), Vec4f(1.0,1.0,1.0,1.0f) );
//        dt.drawLineLoop({a,b,c,d}, 0.5f, Vec4f(.0,1.0,1.0,1.0));
////        dt.drawSphere(p - Vec3d(0,0, squareWidth/2), squareWidth / 7.0, Vec4f(.3f,.3f,.8f,1.0f));
//    }

//    if (!isPicking || pickIndex == 2)
//    {
//        if (!isPicking)
//        {
//            // if we removed the alpha it's for picking
//            glEnable(GL_BLEND);
//            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        }

//        // Draw right plane
//        a = p + Vec3d(squareWidth / 2, -squareWidth / 2.0, -squareWidth / 2);
//        b = a + Vec3d(0.0, 0.0, squareWidth);
//        c = b + Vec3d(0.0, squareWidth, 0.0);
//        d = c + Vec3d(0.0, 0.0, -squareWidth);
//        dt.drawQuad(a,b,c,d, Vec3d(0,0,1), Vec4f(1.0,1.0,1.0,1.0f) );
//        dt.drawLineLoop({a,b,c,d}, 0.5f, Vec4f(.0,1.0,1.0,1.0));
////        glLineWidth(2.0);
////        dt.drawLine(p, p - Vec3d(-squareWidth/2,0, 0), Vec4f(1.0f,.0f,.0f,1.0f));
////        glLineWidth(1.0);
////        dt.drawSphere(p - Vec3d(-squareWidth/2,0, 0), squareWidth / 7.0, Vec4f(1.0f,.0f,.0f,1.0f));
//    }

//    if (!isPicking || pickIndex == 3)
//    {
//        if (!isPicking)
//        {
//            // if we removed the alpha it's for picking
//            glEnable(GL_BLEND);
//            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        }


//        // Draw left plane
//        a = p + Vec3d(-squareWidth / 2.0, -squareWidth / 2.0, -squareWidth /2);
//        b = a + Vec3d(0.0, 0.0, squareWidth);
//        c = b + Vec3d(0.0, squareWidth, 0.0);
//        d = c + Vec3d(0.0, 0.0, -squareWidth);
//        dt.drawQuad(a,b,c,d, Vec3d(0,0,1), Vec4f(1.0,1.0,1.0,1.0f) );
//        dt.drawLineLoop({a,b,c,d}, 0.5f, Vec4f(.0,1.0,1.0,1.0));
////        dt.drawSphere(p - Vec3d(squareWidth/2,0, 0), squareWidth / 7.0, Vec4f(.8f,.3f,.3f,1.0f));
//    }

//    if (!isPicking || pickIndex == 4)
//    {
//        if (!isPicking)
//        {
//            // if we removed the alpha it's for picking
//            glEnable(GL_BLEND);
//            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        }

//        // Draw top plane
//        a = p + Vec3d(-squareWidth / 2.0, squareWidth / 2, -squareWidth / 2);
//        b = a + Vec3d(0.0, 0.0, squareWidth);
//        c = b + Vec3d(squareWidth, 0.0, 0.0);
//        d = c + Vec3d(0.0, 0.0, -squareWidth);
//        dt.drawQuad(a,b,c,d, Vec3d(0,0,1), Vec4f(1.0,1.0,1.0,1.0f) );
//        dt.drawLineLoop({a,b,c,d}, 0.5f, Vec4f(.0,1.0,1.0,1.0));
////        glLineWidth(2.0);
////        dt.drawLine(p, p - Vec3d(0,-squareWidth/2, 0), Vec4f(0.0f,1.0f,.0f,1.0f));
////        glLineWidth(1.0);
////        dt.drawSphere(p - Vec3d(0,-squareWidth/2, 0), squareWidth / 7.0, Vec4f(0.0f,1.0f,.0f,1.0f));
//    }

//    if (!isPicking || pickIndex == 5)
//    {
//        if (!isPicking)
//        {
//            // if we removed the alpha it's for picking
//            glEnable(GL_BLEND);
//            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        }

//        // Draw bottom plane
//        a = p + Vec3d(squareWidth / 2, -squareWidth / 2.0, -squareWidth / 2);
//        b = a + Vec3d(0.0, 0.0, squareWidth);
//        c = b + Vec3d(-squareWidth, 0.0, 0.0);
//        d = c + Vec3d(0.0, 0.0, -squareWidth);
//        dt.drawQuad(a,b,c,d, Vec3d(0,0,1), Vec4f(1.0,1.0,1.0,1.0f) );
//        dt.drawLineLoop({a,b,c,d}, 0.5f, Vec4f(.0,1.0,1.0,1.0));
////        dt.drawSphere(p - Vec3d(0,squareWidth/2, 0), squareWidth / 7.0, Vec4f(.3f,.8f,.3f,1.0f));
//    }
*/
//    glDisable(GL_DEPTH_TEST);
    dt.drawSphere(p, float(squareWidth * 0.8), Vec4f(1.0f, 1.0f, 1.0f, .03f));

    if (!isPicking || pickIndex == 0)
    {
        glLineWidth(3.0);
        dt.drawLine(p, p - Vec3d(0,0, -squareWidth/2), Vec4f(0.30f, 0.53f, 0.94f, 1.0f));
        glLineWidth(1.0);
        dt.drawSphere(p - Vec3d(0,0, -squareWidth/2), float(squareWidth / 7.0), Vec4f(0.30f, 0.53f, 0.94f, 1.0f));
    }

    if (!isPicking || pickIndex == 1)
    {
        dt.drawSphere(p - Vec3d(0,0, squareWidth/2), float(squareWidth / 7.0), Vec4f(0.00f, 0.33f, 0.64f, 1.0f));
    }

    if (!isPicking || pickIndex == 2)
    {
        glLineWidth(3.0);
        dt.drawLine(p, p - Vec3d(-squareWidth/2,0, 0), Vec4f(0.86f, 0.27f, 0.33f, 1.0f));
        glLineWidth(1.0);
        dt.drawSphere(p - Vec3d(-squareWidth/2,0, 0), float(squareWidth / 7.0), Vec4f(0.86f, 0.27f, 0.33f, 1.0f));
    }

    if (!isPicking || pickIndex == 3)
    {
        dt.drawSphere(p - Vec3d(squareWidth/2,0, 0), float(squareWidth / 7.0), Vec4f(0.56f, 0.07f, 0.13f, 1.0f));
    }

    if (!isPicking || pickIndex == 4)
    {
        glLineWidth(3.0);
        dt.drawLine(p, p - Vec3d(0,-squareWidth/2, 0), Vec4f(0.56f, 0.79f, 0.0f, 1.0f));
        glLineWidth(1.0);
        dt.drawSphere(p - Vec3d(0,-squareWidth/2, 0), float(squareWidth / 7.0), Vec4f(0.56f, 0.79f, 0.0f, 1.0f));
    }

    if (!isPicking || pickIndex == 5)
    {
        dt.drawSphere(p - Vec3d(0,squareWidth/2, 0), float(squareWidth / 7.0), Vec4f(0.26f, 0.49f, 0.0f, 1.0f));
    }

    glDisable(GL_MULTISAMPLE_ARB);
    glEnable(GL_LIGHTING);

    cam->setOrthographic(false);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

}

void Viewpoint_Manipulator::mouseClicked(const QPointF& /*mouse*/, SofaViewer *viewer)
{
    Camera* cam = viewer->camera();
    switch (m_index)
    {
    case 0: // move to FRONT plane
        cam->viewFromFront();
        break;
    case 1: // move to BACK plane
        cam->viewFromBack();
        break;
    case 2: // move to LEFT plane
        cam->viewFromRight();
        break;
    case 3: // move to RIGHT plane
        cam->viewFromLeft();
        break;
    case 4: // move to TOP plane
        cam->viewFromTop();
        break;
    case 5: // move to BOTTOM plane
        cam->viewFromBottom();
        break;
//    case 6: // move to BACK plane
//        shift = viewer->projectOnPlane(mouse, pos, cam->direction()) - pos;
//        break;
    };
}

//void Viewpoint_Manipulator::mouseMoved(const QPointF &mouse, SofaViewer *viewer)
//{

//}

//void Viewpoint_Manipulator::mousePressed(const QPointF &mouse, SofaViewer *viewer)
//{

//}

//void Viewpoint_Manipulator::mouseReleased(const QPointF &mouse, SofaViewer *viewer)
//{

//}

QString Viewpoint_Manipulator::getDisplayText() const
{
    return "";
}

int Viewpoint_Manipulator::getIndices() const
{
    return 6 /*+ 8 + 12 + 1*/; // 6 faces, 8 corners, 12 edges + saved viewpoint
}

}  // namespace sofaqtquick

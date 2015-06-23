#include "Manipulator3D_Rotation.h"
#include "Viewer.h"

#include <GL/glew.h>
#include <QMatrix4x4>
#include <qmath.h>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

Manipulator3D_Rotation::Manipulator3D_Rotation(QObject* parent) : Manipulator(parent),
    myAxis("x"),
    myDisplayMark(false),
    myFromMarkAngle(0.0f),
    myToMarkAngle(0.0f)
{

}

Manipulator3D_Rotation::~Manipulator3D_Rotation()
{

}

void Manipulator3D_Rotation::setAxis(QString newAxis)
{
    if(newAxis == myAxis)
        return;

    myAxis = newAxis;

    axisChanged(newAxis);
}


void Manipulator3D_Rotation::setMark(float fromAngle, float toAngle)
{
    myFromMarkAngle = fromAngle;
    myToMarkAngle = toAngle;

    if(myFromMarkAngle > myToMarkAngle)
    {
        myFromMarkAngle = fmod(myFromMarkAngle, 2.0 * M_PI);
        myToMarkAngle   = myToMarkAngle + 2.0 * M_PI;
    }

    myDisplayMark = true;
}

void Manipulator3D_Rotation::unsetMark()
{
    myDisplayMark = false;

    myFromMarkAngle = 0.0f;
    myToMarkAngle = 0.0f;
}

void Manipulator3D_Rotation::draw(const Viewer& viewer) const
{
    Camera* camera = viewer.camera();
    if(!camera)
        return;

    bool xAxis = (-1 != myAxis.indexOf('x'));
    bool yAxis = (-1 != myAxis.indexOf('y'));
    bool zAxis = (-1 != myAxis.indexOf('z'));
    int axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

    if(1 != axisNum)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    QVector4D position = camera->projection() * camera->view() * QVector4D(Manipulator::position(), 1.0);
    position /= position.w();

    glScaled(1.0, 1.0, 0.5);
    glTranslatef(position.x(), position.y(), position.z());
    glScalef(viewer.height() / viewer.width(), 1.0f, 1.0f);
    glMultMatrixf(QMatrix4x4(camera->view().normalMatrix()).constData());

    QVector3D axis = QVector3D(0.0, 0.0, -1.0);
    if(xAxis)
        axis = QVector3D(-1.0, 0.0, 0.0);
    else if(yAxis)
        axis = QVector3D(0.0, -1.0, 0.0);

    bool invertedZ = false;
    if(QVector3D::dotProduct(camera->direction(), axis) < 0.0)
        invertedZ = true;

    // object
    if(xAxis)
        glRotated(90.0, 0.0, 1.0, 0.0);
    else if(yAxis)
        glRotated(-90.0, 1.0, 0.0, 0.0);

    const float radius = 0.15f;
    const float width = 8.0f;

    const int resolution = 64;

    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_ALWAYS);

    glLineWidth(width);
    glEnable(GL_LINE_SMOOTH);

    // draw a ring
    glBegin(GL_LINE_STRIP);
    {
        //front
        if(xAxis)
            glColor3d(1.0, 0.0, 0.0);
        else if(yAxis)
            glColor3d(0.0, 1.0, 0.0);
        else if(zAxis)
            glColor3d(0.0, 0.0, 1.0);

        for(int i = 0; i < resolution; ++i)
        {
            float angle = i / (resolution - 1.0f) * 2.0f * M_PI;
            float alpha = qCos(angle);
            float beta  = qSin(angle);

            glVertex3f(radius * alpha, radius * beta, 0.0);
        }
    }
    glEnd();

    // draw a ring portion to know the delta angle
    if(myDisplayMark)
    {
        // draw a ring
        glBegin(GL_LINE_STRIP);
        {
            //front
            glColor3f(1.0, 1.0, 1.0);

            bool stop = false;
            for(int i = 0; i < resolution; ++i)
            {
                float angle = myFromMarkAngle;

                if(!invertedZ)
                {
                    angle += i / (resolution - 1.0f) * 2.0f * M_PI;

                    if(angle >= myToMarkAngle)
                    {
                        angle = myToMarkAngle;
                        stop = true;
                    }
                }
                else
                {
                    angle -= i / (resolution - 1.0f) * 2.0f * M_PI;

                    if(angle <= myToMarkAngle - 2 * M_PI)
                    {
                        angle = myToMarkAngle - 2 * M_PI;
                        stop = true;
                    }
                }

                float alpha = qSin(angle);
                float beta  = qCos(angle);

                glVertex3f(radius * alpha, radius * beta, 0.0);

                if(stop)
                    break;
            }
        }
        glEnd();
    }

    glDisable(GL_LINE_SMOOTH);
    glLineWidth(1.0f);

    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}

}

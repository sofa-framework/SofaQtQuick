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
    internalDraw(viewer, false);
}

void Manipulator3D_Rotation::pick(const Viewer& viewer) const
{
    internalDraw(viewer, true);
}

void Manipulator3D_Rotation::internalDraw(const Viewer& viewer, bool isPicking) const
{
    if(!visible())
        return;

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

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(position().x(), position().y(), position().z());

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

    const float width = 8.0f;

    float radius = 0.125f;
    {
        QVector4D p0 = camera->projection() * camera->view() * QVector4D(position(), 1.0);
        QVector4D p1 = camera->projection() * camera->view() * QVector4D(position() + camera->right(), 1.0);
        QVector3D direction = ((p1 / p1.w() - p0 / p0.w()).toVector3D());

        radius *= 1.0 / direction.length();
    }

    const int resolution = 64;

    glDisable(GL_CULL_FACE);

    glLineWidth(width);
    glEnable(GL_LINE_SMOOTH);

    QColor color(xAxis ? 255 : 50, yAxis ? 255 : 50, zAxis ? 255 : 50);

    // draw a ring
    glBegin(GL_LINE_STRIP);
    {
        glColor3f(color.redF(), color.greenF(), color.blueF());

        for(int i = 0; i < resolution; ++i)
        {
            float angle = i / (resolution - 1.0f) * 2.0f * M_PI;
            float alpha = qCos(angle);
            float beta  = qSin(angle);

            glVertex3f(radius * alpha, radius * beta, 0.0);
        }
    }
    glEnd();

    if(!isPicking)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glBegin(GL_QUAD_STRIP);
    {
        //front
        for(int i = 0; i < resolution; ++i)
        {
            float angle = i / (resolution - 1.0f) * 2.0f * M_PI;
            float alpha = qCos(angle);
            float beta  = qSin(angle);

            glColor4f(color.redF(), color.greenF(), color.blueF(), 0.0f);
            glVertex3f(0.9 * radius * alpha, 0.9 * radius * beta, 0.0);

            glColor4f(color.redF(), color.greenF(), color.blueF(), 1.0f);
            glVertex3f(radius * alpha, radius * beta, 0.0);
        }
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    {
        //front
        for(int i = 0; i < resolution; ++i)
        {
            float angle = i / (resolution - 1.0f) * 2.0f * M_PI;
            float alpha = qCos(angle);
            float beta  = qSin(angle);

            glColor4f(color.redF(), color.greenF(), color.blueF(), 0.0f);
            glVertex3f(1.1 * radius * alpha, 1.1 * radius * beta, 0.0);

            glColor4f(color.redF(), color.greenF(), color.blueF(), 1.0f);
            glVertex3f(radius * alpha, radius * beta, 0.0);
        }
    }
    glEnd();

    if(!isPicking)
        glDisable(GL_BLEND);

    // draw a ring portion to know the delta angle
    if(myDisplayMark)
    {
        glDepthRange(0.0, 0.9999995);
        glDepthFunc(GL_ALWAYS);

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

        glDepthFunc(GL_LESS);
        glDepthRange(0.0, 1.0);
    }

    glDisable(GL_LINE_SMOOTH);
    glLineWidth(1.0f);

    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}

}

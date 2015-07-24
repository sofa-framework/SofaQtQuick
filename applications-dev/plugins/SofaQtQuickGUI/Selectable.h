#ifndef SELECTABLE_H
#define SELECTABLE_H

#include "SofaQtQuickGUI.h"
#include <QObject>
#include <QVector3D>

namespace sofa
{

namespace qtquick
{

/// QtQuick abstract wrapper for a selectable object
class SOFA_SOFAQTQUICKGUI_API Selectable : public QObject
{
    Q_OBJECT

public:
    Selectable(QObject* parent = 0);
    ~Selectable();

public:
    Q_PROPERTY(QVector3D position READ position)

public:
    const QVector3D& position() const {return myPosition;}
    void setPosition(const QVector3D& position);

private:
    QVector3D myPosition;   ///< the position of the selected object

};

}

}

#endif // SELECTABLE_H

#ifndef SELECTABLE_H
#define SELECTABLE_H

#include "SofaQtQuickGUI.h"
#include <QObject>

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

};

}

}

#endif // SELECTABLE_H

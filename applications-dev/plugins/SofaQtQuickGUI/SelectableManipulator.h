#ifndef SELECTABLE_MANIPULATOR_H
#define SELECTABLE_MANIPULATOR_H

#include "SofaQtQuickGUI.h"
#include "Selectable.h"
#include "Manipulator.h"

namespace sofa
{

namespace qtquick
{

/// QtQuick wrapper for a selectable object representing a Manipulator
class SOFA_SOFAQTQUICKGUI_API SelectableManipulator : public Selectable
{
    Q_OBJECT

public:
    SelectableManipulator(Manipulator& manipulator);
    ~SelectableManipulator();

public:
    Q_PROPERTY(sofa::qtquick::Manipulator* manipulator READ manipulator)

public:
    Manipulator* manipulator() const {return myManipulator;}

private:
    Manipulator* myManipulator;

};

}

}

#endif // SELECTABLE_MANIPULATOR_H

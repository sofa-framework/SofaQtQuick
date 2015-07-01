#include <GL/glew.h>
#include "SelectableManipulator.h"

namespace sofa
{

namespace qtquick
{

SelectableManipulator::SelectableManipulator(Manipulator* manipulator) : Selectable(manipulator),
    myManipulator(manipulator)
{

}

SelectableManipulator::~SelectableManipulator()
{

}

}

}

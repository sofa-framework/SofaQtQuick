#include "Snapping_Manipulator.h"

namespace sofaqtquick
{

Snapping_Manipulator::Snapping_Manipulator(QObject* parent)
{
    m_name = "Snapping_Manipulator";
    m_index = -1;
}

void Snapping_Manipulator::internalDraw(const sofaqtquick::SofaViewer &viewer, int pickIndex, bool isPicking)
{
    return;
}

int Snapping_Manipulator::getIndices() const
{
    return 0;
}

QString Snapping_Manipulator::getDisplayText() const
{
    return "";
}

} // namespace sofaqtquick

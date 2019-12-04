#include "Manipulator.h"

namespace sofaqtquick
{

Manipulator::Manipulator(QObject* parent)
    : QObject(parent)
{

}

const QString& Manipulator::getName()
{
    return m_name;
}

void Manipulator::setName(const QString& name)
{
    m_name = name;
}

int Manipulator::getIndex()
{
    return m_index;
}

void Manipulator::setIndex(int index)
{
    m_index = index;
}

void Manipulator::draw(const SofaViewer& viewer)
{
    int idx = -1;
    internalDraw(viewer, idx);
}

void Manipulator::pick(const SofaViewer& viewer, int& pickIndex)
{
    internalDraw(viewer, pickIndex, true);
}

}  // namespace sofaqtquick

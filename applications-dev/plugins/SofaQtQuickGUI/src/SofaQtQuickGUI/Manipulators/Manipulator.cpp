#include "Manipulator.h"
#include <SofaQtQuickGUI/SofaViewer.h>

namespace sofaqtquick
{

Manipulator::Manipulator(QObject* parent)
    : QObject(parent),
      m_particleIndex(-1),
      m_isEditMode(false),
      m_persistent(false),
      m_enabled(false)
{

}

const QString& Manipulator::getName()
{
    return m_name;
}

void Manipulator::setName(const QString& name)
{
    m_name = name;
    emit nameChanged(m_name);
}

int Manipulator::getIndex()
{
    return m_index;
}

void Manipulator::setIndex(int index)
{
    m_index = index;
    emit indexChanged(m_index);
}

void Manipulator::setParticleIndex(int idx)
{
    m_particleIndex = idx;
    emit particleIndexChanged(m_particleIndex);
}

int Manipulator::getParticleIndex()
{
    return m_particleIndex;
}

bool Manipulator::isEditMode()
{
    return m_isEditMode;
}

void Manipulator::toggleEditMode(bool val)
{
    m_isEditMode = val;
    emit isEditModeChanged(m_isEditMode);
}


void Manipulator::draw(const SofaViewer& viewer)
{
    if (m_enabled) {
        int idx = -1;
        internalDraw(viewer, idx);
    }
}

void Manipulator::pick(const SofaViewer& viewer, int& pickIndex)
{
    if (m_enabled)
        internalDraw(viewer, pickIndex, true);
}

}  // namespace sofaqtquick

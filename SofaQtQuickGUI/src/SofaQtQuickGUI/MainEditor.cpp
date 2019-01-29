#include <MainEditor.h>

namespace sofa
{
namespace qtquick
{

class MainEditorImpl
{
public:
    QList<Manipulator*>     m_manipulators;
    Manipulator*            m_selectedManipulator;

    QList<SofaComponent*>   m_selectedComponents;

    static MainEditorImpl&  singleton() ;
} ;

MainEditorImpl& MainEditorImpl::singleton()
{
    static MainEditorImpl m;
    return m ;
}


void MainEditor::addManipulator(Manipulator* m)
{
    m_impl->m_manipulators.append(m);
}

void MainEditor::removeManipulator(Manipulator* m)
{
    m_impl->m_manipulators.removeAll(m) ;
}

void MainEditor::setSelectedManipulator(Manipulator* m)
{
    m_impl->m_selectedManipulator = m;
}

Manipulator* MainEditor::getSelectedManipulator()
{
    return m_impl->m_selectedManipulator ;
}

void MainEditor::addToSelectedComponents(SofaComponent* c)
{
    m_impl->m_selectedComponents.append(c);
}

void MainEditor::removeFromSelectedComponents(SofaComponent* c)
{
    m_impl->m_selectedComponents.removeAll(c);
}

void MainEditor::removeAllSelectedComponents()
{
    m_impl->m_selectedComponents.clear();
}

const QList<SofaComponent*> MainEditor::getSelectedComponents()
{
    return QList<SofaComponent*>(m_impl->m_selectedComponents) ;
}

} /// qtquick
} /// sofa

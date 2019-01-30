#ifndef SOFAQTQUICKGUI_MAINEDITOR_H
#define SOFAQTQUICKGUI_MAINEDITOR_H

#include <QList>

namespace sofa
{
namespace qtquick
{
class Manipulator ;
class SofaComponent ;
class MainEditorImpl ;

/// Public interface to the 'editor' object which contains all the "static" aspect of
/// the application. This class has no ownership of the component.
class MainEditor
{
public:
    /// Management of the manipulator
    void addManipulator(Manipulator*) ;
    void removeManipulator(Manipulator*) ;
    void setSelectedManipulator(Manipulator*) ;
    Manipulator* getSelectedManipulator() ;

    //// Management of the the selected components
    void addToSelectedComponents(SofaComponent*) ;
    void removeFromSelectedComponents(SofaComponent*) ;
    void removeAllSelectedComponents();
    const QList<SofaComponent*> getSelectedComponents() ; /// https://en.wikipedia.org/wiki/Copy_elision compatible.

    MainEditorImpl* m_impl;
};
} /// qtquick
} /// sofa
#endif /// SOFAQTQUICKGUI_MAINEDITOR_H

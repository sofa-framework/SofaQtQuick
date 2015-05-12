#ifndef PYTHONCONSOLE_H
#define PYTHONCONSOLE_H

#include <SofaPython/PythonMacros.h>

#include "SofaQtQuickGUI.h"
#include <QObject>
#include <QString>
#include <QSet>

namespace sofa
{

namespace qtquick
{

class SOFA_SOFAQTQUICKGUI_API PythonConsole : public QObject
{
    Q_OBJECT

public:
    PythonConsole(QObject *parent = 0);
    ~PythonConsole();

signals:
    void textAdded(const QString& text);

public:
    static void AddText(const QString& text);

private:
    static void InitConsoleRedirection();

private:
    static QSet<PythonConsole*> OurPythonConsoles;

};

}

}

#endif // PYTHONCONSOLE_H

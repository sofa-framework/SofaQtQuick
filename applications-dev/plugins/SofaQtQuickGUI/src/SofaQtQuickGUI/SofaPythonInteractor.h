/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SOFAPYTHONINTERACTOR_H
#define SOFAPYTHONINTERACTOR_H

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
#include <QObject>
#include <QQmlParserStatus>
#include <QMap>
#include <QList>
#include <QString>
#include <QVariant>

#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif

namespace sofa
{

namespace component
{

namespace controller
{
	class PythonScriptController;
}

}

namespace qtquick
{

class SofaScene;
class SofaComponent;

/// \class Allow us to call a sofa PythonScriptController function from QML/JS
class SOFA_SOFAQTQUICKGUI_API SofaPythonInteractor : public QObject, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)

    typedef sofa::component::controller::PythonScriptController PythonScriptController;

public:
    SofaPythonInteractor(QObject *parent = 0);
    ~SofaPythonInteractor();

	void classBegin();
	void componentComplete();
	
public:
    Q_PROPERTY(sofa::qtquick::SofaScene* sofaScene READ sofaScene WRITE setSofaScene /*NOTIFY sofaSceneChanged*/)

    SofaScene* sofaScene() const	{return mySofaScene;}
    void setSofaScene(SofaScene* newScene);

	
//signals:
//    void sofaSceneChanged(sofa::qtquick::SofaScene* newSofaScene);
	
public:
    Q_INVOKABLE bool run(const QString& script);

    /// check if a function exists
    Q_INVOKABLE bool hasFunction(const QString& pythonScriptControllerName, const QString& funcName) const;

    /// call by controller name
    QVariant call(const QString& pythonScriptControllerName, const QString& funcName, const QVariant& parameter = QVariant());

protected:

    PythonScriptController* pythonScriptControllerByName(const QString& pythonScriptControllerName) const;

    /// @internal basic common verifications
    bool onCallBasicVerifications(const QString& funcName, const QVariant& parameter = QVariant());
    /// @internal call by controller
    QVariant onCallByController(PythonScriptController* controller, const QString& funcName, const QVariant& parameter = QVariant());

    /// call by controller wrapped in a SofaComponent
    Q_INVOKABLE QVariant onCallBySofaComponent(sofa::qtquick::SofaComponent* sofaComponent, const QString& funcName, const QVariant& parameter = QVariant());

    /// call by controller name
    Q_INVOKABLE QVariant onCall(const QString& pythonScriptControllerName, const QString& funcName, const QVariant& parameter = QVariant());

private:

    SofaScene* mySofaScene;
	
};

}

}

#endif // SOFAPYTHONINTERACTOR_H

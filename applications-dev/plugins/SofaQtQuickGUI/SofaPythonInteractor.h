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

#include "SofaQtQuickGUI.h"
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

    /// call by controller name
    QVariant call(const QString& pythonScriptControllerName, const QString& funcName, const QVariant& parameter = QVariant());

protected:

    /// @internal basic common verifications
    bool onCallBasicVerifications(const QString& funcName, const QVariant& parameter = QVariant());
    /// @internal call by controller
    QVariant onCallByController(PythonScriptController* controller, const QString& funcName, const QVariant& parameter = QVariant());

    /// call by controller name
    Q_INVOKABLE QVariant onCall(const QString& pythonScriptControllerName, const QString& funcName, const QVariant& parameter = QVariant());

private:

    SofaScene* mySofaScene;
	
};

}

}

#endif // SOFAPYTHONINTERACTOR_H

/*

class SOFA_SOFAPYTHON_API PythonLogger
{
    static PyObject* init(void* rawSelf)
    {
        std::cout << "test" << std::endl;

        PyObject* self = static_cast<PyObject*>(rawSelf);

        int err = PyDict_SetItemString(self, "name2", PyString_FromString("test"));
        //std::cout << "init called : " << PyString_AsString(PyDict_GetItemString(self, "name2")) << std::endl;

        //PyObject* pySysDict = PyModule_GetDict(PyImport_AddModule("sys"));
        //PyObject* pyStdOut = PyDict_GetItemString(pySysDict, "stdout");

        //err = PyDict_SetItemString(self, "name2", PyString_FromString("test2"));
        //PyDict_SetItemString(self, "backup", pyStdOut);

        //PyDict_SetItemString(pySysDict, "stdout", self);

        return self;
    }

    static PyObject* write(void* rawSelf)
    {
        PyObject* self = static_cast<PyObject*>(rawSelf);

        PyObject* name = PyDict_GetItemString(self, "name");
        std::cout << "write: " << PyString_AsString(name) << std::endl;

        return self;
    }

public:
    PythonLogger()
    {
        std::cout << "new" << std::endl;
        myPyObject = PyDict_New();

        std::cout << "a" << std::endl;
        PyDict_SetItemString(myPyObject, "name", PyString_FromString("PythonLogger"));
        std::cout << "b" << std::endl;
        PyObject* pyInitFunction = Py_BuildValue("O&", &PythonLogger::init, myPyObject);
        std::cout << "c" << std::endl;
        PyDict_SetItemString(myPyObject, "test", pyInitFunction);
        std::cout << "d" << std::endl;

        //PyObject* pyWriteFunction = Py_BuildValue("O&", write, myPyObject);
        //PyDict_SetItemString(myPyObject, "write", pyWriteFunction);


//        import sys
//        class CatchOut:
//        def __init__(self):
//        self.value = ''
//        def write(self, txt):
//        self.value += txt
//        catchOut = CatchOut()
//        sys.stdout = catchOut
//        sys.stderr = catchOut

    }

    ~PythonLogger()
    {
        std::cout << "releasing: " << myPyObject << std::endl;
        Py_DECREF(myPyObject);
    }

private:
    PyObject*   myPyObject;

};


*/

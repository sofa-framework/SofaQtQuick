#ifndef PYTHONINTERACTOR_H
#define PYTHONINTERACTOR_H

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

class Scene;

class SOFA_SOFAQTQUICKGUI_API PythonInteractor : public QObject, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)

public:
	PythonInteractor(QObject *parent = 0);
	~PythonInteractor();

	void classBegin();
	void componentComplete();
	
public:
    Q_PROPERTY(sofa::qtquick::Scene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(QList<QString> pythonScriptControllersName READ pythonScriptControllersName NOTIFY pythonScriptControllersNameChanged)

public:
	Scene* scene() const	{return myScene;}
	void setScene(Scene* newScene);

    QList<QString> pythonScriptControllersName() const;
	
signals:
    void sceneChanged(sofa::qtquick::Scene* newScene);
    void pythonScriptControllersNameChanged(const QList<QString>& newPythonScriptControllersName);
	
public:
    Q_INVOKABLE bool run(const QString& script);
    QVariant call(const QString& pythonClassName, const QString& funcName, const QVariant& parameter = QVariant());

protected:
    Q_INVOKABLE QVariant onCall(const QString& pythonClassName, const QString& funcName, const QVariant& parameter = QVariant());

public slots:
	void sendEvent(const QString& pythonClassName, const QString& eventName, const QVariant& parameter = QVariant());
	void sendEventToAll(const QString& eventName, const QVariant& parameter = QVariant());

private slots:
	void handleSceneChanged(Scene* scene);
    void retrievePythonScriptControllers();

private:
	typedef sofa::component::controller::PythonScriptController PythonScriptController;

	Scene*									myScene;
	QMap<QString, PythonScriptController*>	myPythonScriptControllers;
	
};

}

}

#endif // PYTHONINTERACTOR_H

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

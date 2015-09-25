#include "PythonInteractor.h"
#include "Scene.h"
#include "PythonConsole.h"

#include <SofaPython/PythonCommon.h>
#include <SofaPython/PythonEnvironment.h>
#include <SofaPython/ScriptEnvironment.h>
#include <SofaPython/PythonMacros.h>
#include <SofaPython/PythonScriptFunction.h>
#include <SofaPython/PythonScriptController.h>

#include <qqml.h>
#include <QDebug>
#include <QSequentialIterable>
#include <QJSValue>
#include <vector>

namespace sofa
{

namespace qtquick
{

PythonInteractor::PythonInteractor(QObject *parent) : QObject(parent), QQmlParserStatus()
    , myScene(0)
{

}

PythonInteractor::~PythonInteractor()
{
	
}

void PythonInteractor::classBegin()
{

}

void PythonInteractor::componentComplete()
{
	if(!myScene)
		setScene(qobject_cast<Scene*>(parent()));
}

void PythonInteractor::setScene(Scene* newScene)
{
    if(newScene != myScene)
    {
        myScene = newScene;
        //	sceneChanged(newScene); // do we really need to send a signal?
    }
}


static PyObject* PythonBuildValueHelper(const QVariant& parameter)
{
	PyObject* value = 0;
	if(!parameter.isNull())
	{
        QVariant finalParameter = parameter;
        if(finalParameter.userType() == qMetaTypeId<QJSValue>())
            finalParameter = finalParameter.value<QJSValue>().toVariant();

        switch(finalParameter.type())
		{
		case QVariant::Bool:
            value = Py_BuildValue("b", finalParameter.toBool());
			break;
		case QVariant::Int:
            value = Py_BuildValue("i", finalParameter.toInt());
			break;
		case QVariant::UInt:
            value = Py_BuildValue("I", finalParameter.toUInt());
			break;
		case QVariant::Double:
            value = Py_BuildValue("d", finalParameter.toDouble());
			break;
		case QVariant::String:
            value = Py_BuildValue("s", finalParameter.toString().toLatin1().constData());
			break;
		default:
			value = Py_BuildValue("");
            qWarning() << "ERROR: buildPythonParameterHelper, type not supported:" << finalParameter.typeName() << "- id" << finalParameter.type();
			break;
		}
	}

	return value;
}

static PyObject* PythonBuildTupleHelper(const QVariant& parameter, bool mustBeTuple)
{
	PyObject* tuple = 0;

	if(!parameter.isNull())
    {
        QVariant finalParameter = parameter;
        if(finalParameter.userType() == qMetaTypeId<QJSValue>())
            finalParameter = finalParameter.value<QJSValue>().toVariant();

        if(QVariant::List == finalParameter.type())
		{
            QSequentialIterable parameterIterable = finalParameter.value<QSequentialIterable>();
			tuple = PyTuple_New(parameterIterable.size());

			int count = 0;
			for(const QVariant& i : parameterIterable)
				PyTuple_SetItem(tuple, count++, PythonBuildTupleHelper(i, false));
		}
        else if(QVariant::Map == finalParameter.type())
        {
			PyObject* dict = PyDict_New();

            QVariantMap map = finalParameter.value<QVariantMap>();

			for(QVariantMap::const_iterator i = map.begin(); i != map.end(); ++i)
				PyDict_SetItemString(dict, i.key().toLatin1().constData(), PythonBuildTupleHelper(i.value(), false));

			if(mustBeTuple)
			{
				tuple = PyTuple_New(1);
				PyTuple_SetItem(tuple, 0, dict);
			}
			else
			{
				tuple = dict;
			}
		}
		else
		{
			if(mustBeTuple)
			{
				tuple = PyTuple_New(1);
                PyTuple_SetItem(tuple, 0, PythonBuildValueHelper(finalParameter));
			}
			else
			{
                tuple = PythonBuildValueHelper(finalParameter);
			}
		}
	}

	return tuple;
}

static QVariant ExtractPythonValueHelper(PyObject* parameter)
{
	QVariant value;

	if(parameter)
	{
        if PyBool_Check(parameter)
			value = (Py_False != parameter);
		else if(PyInt_Check(parameter))
            value = (int)PyInt_AsLong(parameter);
		else if(PyFloat_Check(parameter))
			value = PyFloat_AsDouble(parameter);
		else if(PyString_Check(parameter))
			value = PyString_AsString(parameter);
	}

	return value;
}

static QVariant ExtractPythonTupleHelper(PyObject* parameter)
{
	QVariant value;

	if(!parameter)
		return value;
	
	if(PyTuple_Check(parameter) || PyList_Check(parameter))
	{
		QVariantList tuple;

		PyObject *iterator = PyObject_GetIter(parameter);
		PyObject *item;

		if(!iterator)
			return value;

        while((item = PyIter_Next(iterator)))
		{
			tuple.append(ExtractPythonTupleHelper(item));

			Py_DECREF(item);
		}
		Py_DECREF(iterator);

		if(PyErr_Occurred())
            qWarning() << "ERROR: during python tuple/list iteration";

		return tuple;
	}
	else if(PyDict_Check(parameter))
	{
		QVariantMap map;

		PyObject* key;
		PyObject* item;
		Py_ssize_t pos = 0;

		while(PyDict_Next(parameter, &pos, &key, &item))
			map.insert(PyString_AsString(key), ExtractPythonTupleHelper(item));

		if(PyErr_Occurred())
            qWarning() << "ERROR: during python dictionary iteration";

		return map;
	}
	else
	{
		value = ExtractPythonValueHelper(parameter);
	}	

	return value;
}

bool PythonInteractor::run(const QString& script)
{
    if(!myScene)
    {
        qWarning() << "ERROR: cannot run Python script on a null scene";
        return false;
    }

    PythonConsole::AddText(">>> " + script + "\n");

    return sofa::simulation::PythonEnvironment::runString(script.toStdString());
}


QVariant PythonInteractor::call(const QString& pythonScriptControllerName, const QString& funcName, const QVariant& parameter)
{
    return onCall(pythonScriptControllerName, funcName, parameter);
}



bool PythonInteractor::onCallBasicVerifications(const QString& funcName, const QVariant& /*parameter*/)
{
    if(!myScene)
    {
        qWarning() << "ERROR: cannot call Python function on a null scene";
        return false;
    }

    if(!myScene->isReady())
    {
        qWarning() << "ERROR: cannot call Python function on a scene that is still loading";
        return false;
    }

    if(funcName.isEmpty())
    {
        qWarning() << "ERROR: cannot call Python function without a valid python function name";
        return false;
    }

    return true;
}


QVariant PythonInteractor::onCallByController(PythonScriptController* pythonScriptController, const QString& funcName, const QVariant& parameter)
{
    PyObject* pyCallableObject = PyObject_GetAttrString(pythonScriptController->scriptControllerInstance(), funcName.toLatin1().constData());
    if(!pyCallableObject)
    {
        qWarning() << "ERROR: cannot call Python function without a valid function name "<<funcName;
        return QVariant();
    }


    sofa::core::objectmodel::PythonScriptFunction pythonScriptFunction(pyCallableObject, true);
    sofa::core::objectmodel::PythonScriptFunctionParameter pythonScriptParameter(PythonBuildTupleHelper(parameter, true), true);
    sofa::core::objectmodel::PythonScriptFunctionResult pythonScriptResult;

    pythonScriptFunction(&pythonScriptParameter, &pythonScriptResult);

    return ExtractPythonTupleHelper(pythonScriptResult.data());
}


QVariant PythonInteractor::onCall(const QString& pythonScriptControllerName, const QString& funcName, const QVariant& parameter)
{
    QVariant result;

    if( !onCallBasicVerifications(funcName,parameter) ) return result;

    if(pythonScriptControllerName.isEmpty())
    {
        qWarning() << "ERROR: cannot call Python function without a python controller path/name";
        return QVariant();
    }

    const char * path = pythonScriptControllerName.toUtf8().constData();
    PythonScriptController* controller = nullptr;

    // try to find by path (faster)
    void* cont = myScene->sofaSimulation()->GetRoot()->getObject( classid(PythonScriptController), path );

    if( cont ) // found by path
    {
       controller = reinterpret_cast<PythonScriptController*>( cont );
    }
    else // try to find by name (slower but more generic)
    {
        std::cerr<<"FINDING BY NAME "<<path<<" "<<funcName.toUtf8().constData()<<std::endl;
        controller = dynamic_cast<PythonScriptController*>( myScene->sofaSimulation()->GetRoot()->getObject( path ) );
    }

    if( !controller )
    {
        qWarning() << "ERROR: cannot call Python function without a valid python controller path/name "<<pythonScriptControllerName;
        return QVariant();
    }

    return onCallByController( controller, funcName, parameter );

    return QVariant();
}


}

}

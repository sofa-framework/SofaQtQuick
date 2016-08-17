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

#include "SofaPythonInteractor.h"
#include "SofaScene.h"
#include "SofaComponent.h"
#include "PythonConsole.h"

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

using namespace sofa::core::objectmodel;
using namespace sofa::component::controller;

SofaPythonInteractor::SofaPythonInteractor(QObject *parent) : QObject(parent), QQmlParserStatus()
    , mySofaScene(0)
{

}

SofaPythonInteractor::~SofaPythonInteractor()
{
	
}

void SofaPythonInteractor::classBegin()
{

}

void SofaPythonInteractor::componentComplete()
{
    if(!mySofaScene)
        setSofaScene(qobject_cast<SofaScene*>(parent()));
}

void SofaPythonInteractor::setSofaScene(SofaScene* newSofaScene)
{
    if(newSofaScene != mySofaScene)
    {
        mySofaScene = newSofaScene;
        //	sofaSceneChanged(newSofaScene); // do we really need to send a signal?
    }
}


static PyObject* PythonBuildValueHelper(const QVariant& parameter)
{
	PyObject* value = 0;

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
        msg_error("SofaQtQuickGUI") << "buildPythonParameterHelper, type not supported:" << finalParameter.typeName() << "- id" << finalParameter.type();
        break;
    }

	return value;
}

static PyObject* PythonBuildTupleHelper(const QVariant& parameter, bool mustBeTuple)
{
	PyObject* tuple = 0;

    if(parameter.isValid())
    {
        QVariant finalParameter = parameter;
        if(finalParameter.userType() == qMetaTypeId<QJSValue>())
            finalParameter = finalParameter.value<QJSValue>().toVariant();

        if(QVariant::List == finalParameter.type())
		{
            QSequentialIterable parameterIterable = finalParameter.value<QSequentialIterable>();
            if(mustBeTuple)
            {
                tuple = PyTuple_New(parameterIterable.size());

                int count = 0;
                for(const QVariant& i : parameterIterable)
                    PyTuple_SetItem(tuple, count++, PythonBuildTupleHelper(i, false));
            }
            else
            {
                PyObject* list = PyList_New(parameterIterable.size());

                int count = 0;
                for(const QVariant& i : parameterIterable)
                    PyList_SetItem(list, count++, PythonBuildTupleHelper(i, false));

                tuple = list;
            }
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

        PyObject* iterator = PyObject_GetIter(parameter);
        if(!iterator)
            return tuple;

        PyObject* item;
        while((item = PyIter_Next(iterator)))
		{
			tuple.append(ExtractPythonTupleHelper(item));

			Py_DECREF(item);
		}
		Py_DECREF(iterator);

		if(PyErr_Occurred())
            msg_error("SofaQtQuickGUI") << "during python tuple/list iteration";

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
            msg_error("SofaQtQuickGUI") << "during python dictionary iteration, key must be of type 'string'";

		return map;
    }
	else
	{
		value = ExtractPythonValueHelper(parameter);
	}	

	return value;
}

bool SofaPythonInteractor::run(const QString& script)
{
    if(!mySofaScene)
    {
        msg_error("SofaQtQuickGUI") << "cannot run Python script on a null SofaScene";
        return false;
    }

    PythonConsole::AddText(">>> " + script + "\n");

    return sofa::simulation::PythonEnvironment::runString(script.toStdString());
}

bool SofaPythonInteractor::hasFunction(const QString& pythonScriptControllerName, const QString& funcName) const
{
    PythonScriptController* pythonScriptController = pythonScriptControllerByName(pythonScriptControllerName);
    if(!pythonScriptController || funcName.isEmpty())
        return false;

    return PyObject_HasAttrString(pythonScriptController->scriptControllerInstance(), funcName.toLatin1().constData());
}

QVariant SofaPythonInteractor::call(const QString& pythonScriptControllerName, const QString& funcName, const QVariant& parameter)
{
    return onCall(pythonScriptControllerName, funcName, parameter);
}

PythonScriptController* SofaPythonInteractor::pythonScriptControllerByName(const QString& pythonScriptControllerName) const
{
    const char* path = pythonScriptControllerName.toLatin1().constData();
    PythonScriptController* controller = nullptr;

    // try to find by path (faster)
    void* rawController = mySofaScene->sofaRootNode()->getObject(classid(PythonScriptController), path);

    if(rawController) // found by path
       controller = reinterpret_cast<PythonScriptController*>(rawController);
    else // try to find by name in the root node (slower but more generic)
        controller = dynamic_cast<PythonScriptController*>(mySofaScene->sofaRootNode()->getObject(path));

    return controller;
}

bool SofaPythonInteractor::onCallBasicVerifications(const QString& funcName, const QVariant& /*parameter*/)
{
    if(!mySofaScene)
    {
        msg_error("SofaQtQuickGUI") << "cannot call Python function on a null SofaScene (" << funcName.toStdString() << ")";
        return false;
    }

    if(!mySofaScene->isReady())
    {
        msg_error("SofaQtQuickGUI") << "cannot call Python function on a SofaScene that is still loading (" << funcName.toStdString() << ")";
        return false;
    }

    if(funcName.isEmpty())
    {
        msg_error("SofaQtQuickGUI") << "cannot call Python function without a valid python function name (" << funcName.toStdString() << ")";
        return false;
    }

    return true;
}

QVariant SofaPythonInteractor::onCallByController(PythonScriptController* pythonScriptController, const QString& funcName, const QVariant& parameter)
{
    PyObject* pyCallableObject = PyObject_GetAttrString(pythonScriptController->scriptControllerInstance(), funcName.toLatin1().constData());
    if(!pyCallableObject)
    {
        msg_error("SofaQtQuickGUI") << "cannot call Python function without a valid function name (" << pythonScriptController->getName() << "::" << funcName.toStdString() << ")";
        return QVariant();
    }

//    // if there are paramaters, they are packed into a list and we have to unpack them first
//    QVariant finalParameter = parameter;
//    if(qMetaTypeId<QJSValue>() == finalParameter.userType())
//        finalParameter = finalParameter.value<QJSValue>().toVariant();

//    if(QVariant::List == finalParameter.type())
//        finalParameter = finalParameter.value<QList<QVariant>>().first();

//    qDebug() << "Type" << finalParameter.typeName();
//    qDebug() << funcName << " = " << parameter << " ; " << finalParameter;

    sofa::core::objectmodel::PythonScriptFunction pythonScriptFunction(pyCallableObject, true);
    sofa::core::objectmodel::PythonScriptFunctionParameter pythonScriptParameter(PythonBuildTupleHelper(parameter, true), true);
    sofa::core::objectmodel::PythonScriptFunctionResult pythonScriptResult;

    pythonScriptFunction(&pythonScriptParameter, &pythonScriptResult);

    return ExtractPythonTupleHelper(pythonScriptResult.data());
}

QVariant SofaPythonInteractor::onCallBySofaComponent(SofaComponent* sofaComponent, const QString& funcName, const QVariant& parameter)
{
    if(!onCallBasicVerifications(funcName, parameter))
        return QVariant();

    Base* base = sofaComponent->base();
    if(!base)
        return QVariant();

    PythonScriptController* controller = dynamic_cast<PythonScriptController*>(base);
    if(!controller)
        return QVariant();

    return onCallByController(controller, funcName, parameter);
}

QVariant SofaPythonInteractor::onCall(const QString& pythonScriptControllerName, const QString& funcName, const QVariant& parameter)
{
    if(!onCallBasicVerifications(funcName, parameter))
        return QVariant();

    if(pythonScriptControllerName.isEmpty())
    {
        msg_error("SofaQtQuickGUI") << "cannot call Python function without a python controller path/name";
        return QVariant();
    }

    PythonScriptController* controller = pythonScriptControllerByName(pythonScriptControllerName);
    if(!controller)
    {
        msg_error("SofaQtQuickGUI") << "cannot call Python function (" << funcName.toStdString() << ") without a valid python controller path/name (" << pythonScriptControllerName.toStdString() << ")";
        return QVariant();
    }

    return onCallByController(controller, funcName, parameter);
}

}

}

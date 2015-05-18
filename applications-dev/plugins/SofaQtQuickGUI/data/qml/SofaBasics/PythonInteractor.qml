import QtQuick 2.0
import PythonInteractor 1.0

PythonInteractor {
    id: root

    function call(scriptControllerClassName, funcName) {
        if(arguments.length == 2) {
            return onCallByClassName(scriptControllerClassName, funcName);
        } else if(arguments.length > 2){
            var packedArguments = [];
            for(var i = 2; i < arguments.length; i++)
                packedArguments.push(arguments[i]);

            return onCallByClassName(scriptControllerClassName, funcName, packedArguments);
        }

        console.debug("ERROR: PythonInteractor - using call with an invalid number of arguments:", arguments.length);
    }

    function callByControllerName(scriptControllerName, funcName) {
        if(arguments.length == 2) {
            return onCallByControllerName(scriptControllerName, funcName);
        } else if(arguments.length > 2){
            var packedArguments = [];
            for(var i = 2; i < arguments.length; i++)
                packedArguments.push(arguments[i]);

            return onCallByControllerName(scriptControllerName, funcName, packedArguments);
        }

        console.debug("ERROR: PythonInteractor - using call with an invalid number of arguments:", arguments.length);
    }
}

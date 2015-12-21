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

import QtQuick 2.0
import SofaPythonInteractor 1.0

SofaPythonInteractor {
    id: root

    function call(scriptControllerName, funcName) {
        //console.log(arguments.length);
        if(arguments.length == 2) {
            return onCall(scriptControllerName, funcName);
        } else if(arguments.length > 2) {
            var packedArguments = [];
            for(var i = 2; i < arguments.length; ++i)
                packedArguments.push(arguments[i]);

            return onCall(scriptControllerName, funcName, packedArguments);
        }

        console.debug("ERROR: SofaPythonInteractor - using call with an invalid number of arguments:", arguments.length);
    }
}

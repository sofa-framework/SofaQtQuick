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

.pragma library

function InstanciateComponent(component, parent, properties, verbose) {
    if(!parent)
        parent = null;

    if(!properties)
        properties = null;

    if(undefined === verbose)
        verbose = true;

    if(3 === component.status) {
        if(verbose)
            console.warn("ERROR creating component:", component.errorString());

        return null;
    }

    var incubator = component.incubateObject(parent, properties);
    incubator.forceCompletion();

    return incubator.object;
}

function InstanciateURLComponent(url, parent, properties, verbose) {
    if(!parent)
        parent = null;

    if(!properties)
        properties = null;

    if(undefined === verbose)
        verbose = true;

    var component = Qt.createComponent(url);
    if(3 === component.status) {
        if(verbose)
            console.warn("ERROR creating component:", component.errorString());

        return null;
    }

    var incubator = component.incubateObject(parent, properties);
    incubator.forceCompletion();

    return incubator.object;
}

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
import SofaApplication 1.0
import "."

/*
  \brief MementoObject define a collection of properties from a specific abstraction level to be saved and restored
         You must register every properties you want to save using the registerProperty function providing the id of the object
         containing the property, the property name, it's get / set function if needed (e.g if you need to convert values or add specific actions because by default we will use the assignation operator)
*/
QtObject {
    id: root

    property bool enabled: true
    property int level: SofaApplication.objectDepthFromRoot(this);
    readonly property alias properties: d.properties

    Component.onCompleted: {
        MementoManager.addMementoObject(this);
    }

    Component.onDestruction: {
        MementoManager.removeMementoObject(this);
    }

    // final
    function registerProperty(id, name, get, set) {
        d.properties.push({'id': id, 'name': name, 'get': get, 'set': set});
        // TODO: check if changed signal is sent or not
    }

    // virtual
    function getProperties() {
        return defaultGetProperties();
    }

    // virtual
    function setProperties(data) {
        defaultSetProperties(data);
    }

    // final
    function defaultGetProperties() {
        var data = [];

        if(d.properties) {
            for(var i = 0; i < d.properties.length; ++i) {
                var property = d.properties[i];

                var name = property.name;
                var value;

                var get = property.get;
                if(get)
                    value = get();
                else
                    value = property.id[name];

                value = JSON.parse(JSON.stringify(value)); // deep copy to avoid reference on modifiable objects (if the value is modified the snapshot would be too)

                data.push({'name': name, 'value': value});
            }
        }

        return data;
    }

    // final
    function defaultSetProperties(data) {
        if(!d.properties)
            return;

        if(data) {
            for(var i = 0; i < data.length; ++i) {
                var datum = data[i];

                var name = datum.name;
                var value = datum.value;

                value = JSON.parse(JSON.stringify(value)); // deep copy to avoid reference on modifiable objects (if the value is modified the snapshot would be too)

                var property = d.properties[i];

                var set = property.set;
                if(set)
                    set(value);
                else
                    property.id[name] = value;
            }
        }
    }

    property var d : QtObject {
        id: d

        property var properties: []
    }
}

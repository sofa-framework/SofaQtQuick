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

import Qt.labs.settings 1.0
import "qrc:/SofaCommon/SofaToolsScript.js" as SofaToolsScript

Settings {
    category: "ui"

    property string uiIds: ";"

    function generate() {
        var uiId = 1;
        var uiIdList = uiIds.split(";");

        var previousId = 0;
        for(var i = 0; i < uiIdList.length; ++i) {
            if(0 === uiIdList[i].length)
                continue;

            uiId = Number(uiIdList[i]);

            if(previousId + 1 !== uiId) {
                uiId = previousId + 1;
                break;
            }

            previousId = uiId;
            ++uiId;
        }

        return uiId;
    }

    function add(uiId) {
        if(0 === uiId)
            return;

        if(-1 === uiIds.search(";" + uiId.toString() + ";")) {
            uiIds += uiId.toString() + ";";
            update();
        }
    }

    function remove(uiId) {
        if(0 === uiId)
            return;

        uiIds = uiIds.replace(";" + uiId.toString() + ";", ";");

        SofaToolsScript.Tools.clearSettingGroup("ui_" + uiId.toString());
    }

    function replace(previousUiId, uiId) {
        if(0 === uiId)
            return;

        uiIds = uiIds.replace(";" + uiId.toString() + ";", ";");

        if(-1 === uiIds.search(";" + uiId.toString() + ";")) {
            uiIds += uiId.toString() + ";";
            update();
        }
    }

    function update() {
        var uiIdList = uiIds.split(";");
        uiIdList.sort(function(a, b) {return Number(a) - Number(b);});

        uiIds = ";";
        for(var i = 0; i < uiIdList.length; ++i)
            if(0 !== uiIdList[i].length)
                uiIds += uiIdList[i] + ";";
    }
}

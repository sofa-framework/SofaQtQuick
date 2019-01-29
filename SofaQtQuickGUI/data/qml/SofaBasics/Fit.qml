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

Item {
    implicitWidth: {
    	var width = 0;
    	for(var i = 0; i < children.length; ++i)
    		width = Math.max(width, children[i].implicitWidth);

		return width;
    }

    implicitHeight: {
    	var height = 0;
    	for(var i = 0; i < children.length; ++i)
    		height = Math.max(height, children[i].implicitHeight);

		return height;
    }
}

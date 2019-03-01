/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/
import QtQuick 2.0

/// QmlGTestCase is a component implementing the basic test infrastructure
/// to connect QTest&QML test with gtest.
Item
{
    function assert_true(a)
    {
        if(!a)
            console.exception("FAILURE: expecting 'true' got '"+a+"'");
    }

    function assert_false(a)
    {
        if(a)
            console.exception("FAILURE: expecting 'false' got "+a+"'");
    }

    function assert_eq(a,b)
    {
        if(a!==b)
            console.exception("FAILURE: expecting "+a+" got "+b);
    }

    function assert_neq(a,b)
    {
        if(a===b)
            console.exception("FAILURE: expecting '"+b+"' not '"+a+"'");
    }

    function assert_isNull(a)
    {
        if(a===null)
            console.exception("FAILURE: expecting 'null' got "+a);
    }

    function assert_isNotNull(a)
    {
        if(a!==null)
            console.exception("FAILURE: expecting not 'null' but got "+a);
    }

    function assert_isUndefined(a)
    {
        if(a===undefined)
            console.exception("FAILURE: expecting 'undefined' got "+a);
    }

    function assert_isDefined(a)
    {
        if(a!==undefined)
            console.exception("FAILURE: expecting not 'undefined' but got "+a);
    }

}

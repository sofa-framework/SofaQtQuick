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
            console.exception("FAILURE: expecting 'true' got '"+a+"("+typeof(a)+")'");
    }

    function assert_false(a)
    {
        if(a)
            console.exception("FAILURE: expecting 'false' got "+a+"("+typeof(a)+")'");
    }

    function assert_eq(a,b)
    {
        if(a!==b)
            console.exception("FAILURE: expecting '"+a+"("+typeof(a)+")'"+" got "+b+"("+typeof(b)+")'");
    }

    function assert_neq(a,b)
    {
        if(a===b)
            console.exception("FAILURE: expecting '"+b+"("+typeof(b)+")' not '"+a+"("+typeof(a)+")'");
    }

    function assert_isNull(a)
    {
        if(a===null)
            console.exception("FAILURE: expecting 'null' got "+a+"("+typeof(a)+")'");
    }

    function assert_isNotNull(a)
    {
        if(a!==null)
            console.exception("FAILURE: expecting not 'null' but got "+a+"("+typeof(a)+")'");
    }

    function assert_isUndefined(a)
    {
        if(a===undefined)
            console.exception("FAILURE: expecting 'undefined' got "+a+"("+typeof(a)+")'");
    }

    function assert_isDefined(a)
    {
        if(a!==undefined)
            console.exception("FAILURE: expecting not 'undefined' but got "+a+"("+typeof(a)+")'");
    }

    function assert_throw(f)
    {
        var hasCatch=false;
        try
        {
            f();
        }catch(error)
        {
            hasCatch=true;
        }

        if(hasCatch!==true){
            console.exception("FAILURE: expecting function to throw an exception. Got None.");
        }
    }

}

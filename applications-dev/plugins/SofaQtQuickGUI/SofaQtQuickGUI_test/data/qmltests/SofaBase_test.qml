import QtQuick 2.12

import Sofa.Core.Node 1.0
import Sofa.Core.Base 1.0

Item
{
    function assert_eq(a,b)
    {
        if(a!==b)
            console.exception("FAILURE: expecting "+a+" got "+b);
    }

    function expect_eq(a, b)
    {
        console.log("COUCOU ... affiche");
        console.exception("FAILURE: expecting "+a+" got "+b);
    }

    function tst_math()
    {
        console.log("CA MARCHE");
        assert_eq(4,3);
    }
}

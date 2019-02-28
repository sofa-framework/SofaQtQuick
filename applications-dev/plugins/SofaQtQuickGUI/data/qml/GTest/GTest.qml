import QtQuick 2.0

Item
{
    id: control

    function ASSERT_EQ(a,b)
    {
        if(a!==b)
            console.exception("FAILURE: expecting "+a+" got "+b);
    }
}

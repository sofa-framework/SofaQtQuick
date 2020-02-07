    /*******************************************************************
    Copyright 2019, CNRS

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
    *******************************************************************/
    /*******************************************************************
     * Contributors:
     *   - damien.marchal@univ-lille.fr (CNRS)
     *******************************************************************/
    import QtQuick 2.11
    import QtQuick.Controls 2.11

    /// Error view to indicate that something was wrong while loading the view.
    Rectangle {
        color: "transparent"
        anchors.fill: parent
        property string errorMessage : ""

        Label {
            anchors.centerIn: parent
            color: "red"
            width: parent.width - 20
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            text: "An error occurred, the content could not be loaded ! Reason: \n" + errorMessage
            wrapMode: Text.WordWrap
            font.bold: true
        }
    }



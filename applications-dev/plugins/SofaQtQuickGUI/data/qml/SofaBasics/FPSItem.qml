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

import QtQuick 2.3
import QtQuick.Controls 2.4

Label {
    id: root
    text: "FPS: " + d.timer.fps

    onEnabledChanged: {
        d.timer.running=enabled;
        if (!enabled) {
            d.timer.fps = 0;
        }
    }

    QtObject {
        id : d

        property Timer timer: Timer {
            running: true
            repeat: true
            interval: 1

            property int frameCount: 0
            property int previousTime: 0
            property int fps: 0
            onTriggered: {
                frameCount++;
                var currentTime = new Date().getSeconds();

                if(currentTime > previousTime)
                {
                    previousTime = currentTime;
                    fps = frameCount;
                    frameCount = 0;
                }
                else
                {
                    previousTime = currentTime;
                }
            }
        }
    }
}

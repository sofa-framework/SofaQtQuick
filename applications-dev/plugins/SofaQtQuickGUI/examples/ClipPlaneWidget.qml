import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import SofaBasics 1.0

SofaSceneInterface {
    id: root
    toolpanel:
         ClipPlane {
             clipPlaneComponentPath: "/clipPlane"
             distanceMin: -10
             distanceMax: 10
             distanceStepSize: 0.1
         }
}
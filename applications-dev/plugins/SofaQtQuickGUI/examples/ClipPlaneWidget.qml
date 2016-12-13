import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import SofaBasics 1.0

SofaSceneInterface {
    id: root
    toolpanel:
         ClipPlane {
             clipPlaneComponentPath: "/clipPlane"
             distanceMinMax: 10
         }
}
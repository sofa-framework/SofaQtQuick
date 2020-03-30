import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4 as QQCS1
import QtGraphicalEffects 1.12
import QtCharts 2.3
import SofaApplication 1.0
import SofaBasics 1.0
import SofaColorScheme 1.0
import ProfilerTreeViewModel 1.0

Window {
    id: profiler
    onVisibleChanged: {
        profilerModel.activateTimer(true)
    }

    property var rootNode : SofaApplication.sofaScene.rootNode
    onRootNodeChanged: {
        time = rootNode.getData("time")
    }
    property var time
    Connections {
        target: time
        property var stepNumber: 0
        onValueChanged: {
            stepNumber++
            stepsDuration.updateChart(stepNumber, profilerModel.recordStep(stepNumber))
        }
    }

    visible: true
    width: 1000
    height: 800
    title: "Profiler"
    Rectangle {
        anchors.fill: parent
        color: "lightskyblue"
        ChartView {
            id: stepsDuration
            title: "Steps duration (in ms)"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height / 2
            antialiasing: true
            animationOptions: ChartView.NoAnimation
            theme: ChartView.ChartThemeDark
            opacity: 0.7

            ValueAxis{
                id: axisX
                labelFormat: "%d"
                tickType: ValueAxis.TicksDynamic
                tickAnchor: 0
                tickInterval: 10
            }
            ValueAxis{
                id: axisY
                min: 0
                max: 100
                tickType: ValueAxis.TicksDynamic
                tickAnchor: 0
                tickInterval: 100
                minorTickCount: 10
            }

            function roundUp(val, multiple) {
                if (multiple === 0)
                    return val;

                var remainder = val % multiple;
                if (remainder === 0)
                    return val;

                return val + multiple - remainder;
            }

            function updateChart(step, duration) {
                fullAnimationStep.append(step, duration)
                if (step > 99) {
                    fullAnimationStep.remove(0)
                    axisX.min = fullAnimationStep.at(0).x
                    axisX.max = fullAnimationStep.at(99).x
                }
                if (duration > axisY.max) {
                    axisY.max = roundUp(duration, 100)
                }
            }

            LineSeries {
                id: fullAnimationStep
                name: "Full Animation Step"
                axisX: axisX
                axisY: axisY
                color: "red"
                width: 2
                capStyle: Qt.SquareCap

                Component.onCompleted: {
                    append(0,0)
                    axisX.min = 0
                    axisX.max = 100
                }
            }



            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var p = Qt.point(mouse.x, mouse.y);
                    var value = stepsDuration.mapToValue(p, fullAnimationStep);
                    verticalCursorBar.clear()
                    verticalCursorBar.append(fullAnimationStep.at(value.x - axisX.min).x, axisY.min)
                    verticalCursorBar.append(fullAnimationStep.at(value.x - axisX.min).x, axisY.max)
                }
            }
            LineSeries {
                id: verticalCursorBar
                axisX: axisX
                axisY: axisY
                color: "black"
                width: 2
            }
        }

        Rectangle {
            id: treeViewParent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: stepsDuration.bottom
            anchors.bottom: parent.bottom
            anchors.margins: 9
            anchors.topMargin: 0
            color: SofaApplication.style.contentBackgroundColor
            radius: 5

            TreeView {
                id: treeView
                model: ProfilerTreeViewModel {
                    id: profilerModel
                    bufferSize: 100
                }

                itemDelegate: Rectangle {
                    id: delegate
                    height: 20
                    Text {
                        anchors.fill: parent
                        text: label
                    }
                }
            }
        }
    }
}

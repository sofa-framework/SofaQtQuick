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
        print("onVisibleChanged")
        profilerModel.activateTimer(true)
        stepsDuration.clearChart();
        stepsDuration.updateChart(0,0)
    }

    property var rootNode : SofaApplication.sofaScene.rootNode
    onRootNodeChanged: {
        time = rootNode.getData("time")
        dt = rootNode.getData("dt")
        stepsDuration.clearChart()
        stepsDuration.updateChart(0,0)
    }
    property var time
    property var dt
    property var stepNumber: time.value / dt.value
    property var oldStepNb
    onStepNumberChanged: {
        if (profiler.visible && oldStepNb !== stepNumber) {
            while(oldStepNb < stepNumber - 2) {
                print("old:" + Number(++oldStepNb).toString())
                stepsDuration.updateChart(oldStepNb, profilerModel.recordStep(stepNumber))
            }
            print("new:" + Number(stepNumber).toString())
            stepsDuration.updateChart(stepNumber, profilerModel.recordStep(stepNumber))
            oldStepNb = stepNumber
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
                print("step: " + Number(step).toString())
                print("duration: " + Number(duration).toString())
                fullAnimationStep.append(step, duration)
                if (fullAnimationStep.count > profilerModel.bufferSize) {
                    fullAnimationStep.remove(0)
                    axisX.min = fullAnimationStep.at(0).x
                }
                axisX.max = fullAnimationStep.at(fullAnimationStep.count -1).x
                if (duration > axisY.max) {
                    axisY.max = roundUp(duration, 100)
                }
            }

            function clearChart() {
                var currentStep = time.value / rootNode.getData("dt").value

                axisX.min = currentStep
                axisX.max = currentStep + profilerModel.bufferSize
                profilerModel.clearBuffer();
                profilerModel.seek(0)
                verticalCursorBar.clear()
                verticalCursorBar.append(axisX.min, axisY.min)
                verticalCursorBar.append(axisX.min, axisY.max)
                fullAnimationStep.clear()
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
                    axisX.max = profilerModel.bufferSize
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var p = Qt.point(mouse.x, mouse.y);
                    var value = stepsDuration.mapToValue(p, fullAnimationStep);
                    verticalCursorBar.clear()
                    verticalCursorBar.append(Math.round(value.x), axisY.min)
                    verticalCursorBar.append(Math.round(value.x), axisY.max)
                    var a = Number(value.x)
                    var b = Number(axisX.min)
                    profilerModel.seek(Number(a - b))
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

                anchors.fill: parent
                anchors.margins: 10
                verticalScrollBarPolicy: Qt.ScrollBarAsNeeded
                alternatingRowColors: true

                model: ProfilerTreeViewModel {
                    id: profilerModel
                    bufferSize: 100
                }

                TableViewColumn {
                    role: "step"
                    title: "Step"
                }

                TableViewColumn {
                    role: "label"
                    title: "Step ID"
                }

                TableViewColumn {
                    role: "selfMs"
                    title: "Sub-step duration"
                }
                TableViewColumn {
                    role: "totalMs"
                    title: "Total step duration"
                }
                TableViewColumn {
                    role: "totalPerc"
                    title: "total"
                }

                itemDelegate: Rectangle {
                    id: delegate
                    height: 20
                    color: "transparent"
                    Text {
                        anchors.fill: parent
                        text: styleData.value
                        clip: true
                    }
                }

                rowDelegate: Rectangle {
                    color: styleData.selected ? "#82878c" : styleData.alternate ? SofaApplication.style.alternateBackgroundColor : SofaApplication.style.contentBackgroundColor
                }

                headerDelegate: Rectangle {
                    implicitHeight: 20
                    color: "transparent"
                    border.color: "black"
                    property var pressed: styleData.pressed
                    onPressedChanged: forceActiveFocus()
                    Label {
                        id: hdrlbl
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5

                        color: "black"
                        text: styleData.value
                        clip: true
                        elide: "ElideRight"
                    }
                }

                style: QQCS1.TreeViewStyle {
                    branchDelegate: ColorImage {
                        id: groupBoxArrow
                        y: 1
                        source: styleData.isExpanded ? "qrc:/icon/downArrow.png" : "qrc:/icon/rightArrow.png"
                        width: 14
                        height: 14
                        color: "#393939"
                    }
                    backgroundColor: SofaApplication.style.contentBackgroundColor

                    scrollBarBackground: GBRect {
                        border.color: "#3f3f3f"
                        radius: 6
                        implicitWidth: 12
                        implicitHeight: 12
                        LinearGradient {
                            cached: true
                            source: parent
                            anchors.left: parent.left
                            anchors.leftMargin: 1
                            anchors.right: parent.right
                            anchors.rightMargin: 1
                            anchors.top: parent.top
                            anchors.topMargin: 0
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 0
                            start: Qt.point(0, 0)
                            end: Qt.point(12, 0)
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#565656" }
                                GradientStop { position: 1.0; color: "#5d5d5d" }
                            }
                        }
                        isHorizontal: true
                        borderGradient: Gradient {
                            GradientStop { position: 0.0; color: "#444444" }
                            GradientStop { position: 1.0; color: "#515151" }
                        }
                    }

                    corner: null
                    scrollToClickedPosition: true
                    transientScrollBars: false
                    frame: Rectangle {
                        color: "transparent"
                    }

                    handle: Item {
                        implicitWidth: 12
                        implicitHeight: 12
                        GBRect {
                            radius: 6
                            anchors.fill: parent
                            border.color: "#3f3f3f"
                            LinearGradient {
                                cached: true
                                source: parent
                                anchors.left: parent.left
                                anchors.leftMargin: 1
                                anchors.right: parent.right
                                anchors.rightMargin: 1
                                anchors.top: parent.top
                                anchors.topMargin: 0
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: 0

                                start: Qt.point(0, 0)
                                end: Qt.point(12, 0)
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: "#979797" }
                                    GradientStop { position: 1.0; color: "#7b7b7b" }
                                }
                            }
                            isHorizontal: true
                            borderGradient: Gradient {
                                GradientStop { position: 0.0; color: "#444444" }
                                GradientStop { position: 1.0; color: "#515151" }
                            }

                        }
                    }
                    incrementControl: Rectangle {
                        visible: false
                    }
                    decrementControl: Rectangle {
                        visible: false
                    }
                }
            }
        }
    }
}

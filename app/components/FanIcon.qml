pragma ComponentBehavior: Bound

import QtQuick

Item {
    id: fanIcon

    property real rpm: 0
    property bool active: true

    readonly property real maxRpm: 10000
    readonly property real maxRevsPerSecond: 2

    property real speed: Math.min(Math.max(rpm, 0), maxRpm) / maxRpm * maxRevsPerSecond
    Behavior on speed { NumberAnimation { duration: 1500 } }

    readonly property int bladeCount: 14
    readonly property real hubRadius: 6
    readonly property real hubInnerRadius: 3
    readonly property real bladeLength: 6.5
    readonly property real bladeTilt: 50

    implicitWidth: Theme.iconSize
    implicitHeight: Theme.iconSize

    Item {
        id: blades
        anchors.fill: parent

        Repeater {
            model: fanIcon.bladeCount
            delegate: Item {
                required property int index

                x: blades.width / 2
                y: blades.height / 2
                rotation: index * 360 / fanIcon.bladeCount

                Rectangle {
                    x: fanIcon.hubRadius
                    y: -height / 2
                    width: fanIcon.bladeLength
                    height: Theme.lineWidth
                    radius: height / 2
                    color: Theme.text
                    antialiasing: true
                    transformOrigin: Item.Left
                    rotation: fanIcon.bladeTilt
                }
            }
        }
    }

    Repeater {
        model: [fanIcon.hubRadius, fanIcon.hubInnerRadius]
        delegate: Rectangle {
            required property real modelData

            anchors.centerIn: fanIcon
            width: modelData * 2
            height: width
            radius: width / 2
            color: "transparent"
            border.color: Theme.text
            border.width: Theme.lineWidth
            antialiasing: true
        }
    }

    FrameAnimation {
        running: fanIcon.active && fanIcon.speed > 0.001
        onTriggered: blades.rotation = (blades.rotation + fanIcon.speed * 360 * frameTime) % 360
    }
}

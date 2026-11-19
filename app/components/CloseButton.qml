import QtQuick

// Крестик закрытия: две линии по центру круга.
Rectangle {
    signal clicked()

    width: 28
    height: 28
    radius: 14
    color: mouse.containsMouse ? Theme.border : "transparent"

    Repeater {
        model: [45, -45]
        delegate: Rectangle {
            anchors.centerIn: parent
            width: 12
            height: 2
            radius: 1
            rotation: modelData
            antialiasing: true
            color: Theme.text
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: parent.clicked()
    }
}

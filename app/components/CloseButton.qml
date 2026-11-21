import QtQuick

Rectangle {
    id: closeButton

    signal clicked()

    width: Theme.closeButtonSize
    height: width
    radius: width / 2
    color: mouse.containsMouse ? Theme.border : "transparent"

    Icon {
        anchors.centerIn: closeButton
        path: Theme.iconClose
    }

    MouseArea {
        id: mouse
        anchors.fill: closeButton
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: closeButton.clicked()
    }
}

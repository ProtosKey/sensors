import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import "components"
import "cards"

Window {
    id: root

    width: Math.round(Theme.windowWidth * Theme.fontScale)
    height: background.height

    visible: true
    title: "Sensors"
    color: "transparent"

    readonly property bool wayland: Qt.platform.pluginName.startsWith("wayland")
    flags: Qt.FramelessWindowHint | (wayland ? 0 : Qt.WindowStaysOnTopHint)

    Component.onCompleted: {
        if (!wayland) {
            x = Screen.desktopAvailableWidth - width - Theme.windowScreenMargin
            y = Screen.desktopAvailableHeight - height - Theme.windowScreenMargin
        }
    }

    readonly property bool windowActive: visible && visibility !== Window.Minimized

    Shortcut {
        sequence: "Escape"
        onActivated: Qt.quit()
    }

    FansModel {
        id: fans
        source: socket.fans
    }

    Rectangle {
        id: background
        width: parent.width
        height: content.implicitHeight + 2 * Theme.windowBorder + 2
        radius: Theme.windowRadius
        color: Theme.border

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onPressed: root.startSystemMove()
        }

        Rectangle {
            id: content
            anchors.fill: parent
            anchors.margins: Theme.windowBorder
            radius: Theme.windowRadius - 2 * Theme.windowBorder
            color: Theme.window
            implicitHeight: column.implicitHeight + 2 * Theme.windowPadding

            ColumnLayout {
                id: column
                anchors.fill: parent
                anchors.margins: Theme.windowPadding
                spacing: Theme.sectionSpacing

                Text {
                    text: qsTr("Sensors")
                    color: Theme.text
                    font.pointSize: Theme.sizeHeader
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    visible: !socket.connected
                    text: qsTr("Daemon not connected")
                    color: Theme.error
                    font.pointSize: Theme.sizeNote
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: Theme.noteMargin
                }

                ProfileCard {
                    visible: socket.hasData
                    profile: socket.profile
                }
                CpuCard {
                    visible: socket.hasData
                    cpu: socket.cpu
                }

                Repeater {
                    model: socket.gpu
                    delegate: GpuCard {
                        gpu: modelData
                    }
                }

                Repeater {
                    model: fans
                    delegate: FanCard {
                        label: model.label
                        rpm: model.rpm
                        is_active: root.windowActive
                    }
                }

                RamCard {
                    ram: socket.ram
                }
            }
        }
    }

    CloseButton {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: Theme.closeButtonMargin
        anchors.rightMargin: Theme.closeButtonMargin
        onClicked: Qt.quit()
    }
}

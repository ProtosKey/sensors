import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import "components"
import "cards"

Window {
    id: root
    width: 340
    height: background.height

    visible: true
    title: "Sensors"
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    // Начальная позиция (работает на X11, на Wayland проигнорируется)
    x: Screen.desktopAvailableWidth - width - 20
    y: Screen.desktopAvailableHeight - height - 20

    // Анимации крутятся только пока окно видно
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
        height: content.implicitHeight + 4
        radius: 15
        color: Theme.mid

        // Перетаскивание окна за любое пустое место
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onPressed: root.startSystemMove() // работает в KDE/Wayland
        }

        Rectangle {
            id: content
            anchors.fill: parent
            anchors.margins: 1
            radius: 13
            color: Theme.window
            implicitHeight: column.implicitHeight + 30

            ColumnLayout {
                id: column
                anchors.fill: parent
                anchors.margins: 15
                spacing: 8

                Text {
                    text: qsTr("Sensors")
                    color: Theme.text
                    font.pixelSize: 18
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    visible: !socket.connected
                    text: qsTr("Daemon not connected")
                    color: Theme.error
                    font.pixelSize: 11
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 4
                }

                ProfileCard { profile: socket.profile }
                CpuCard { cpu: socket.cpu }

                Repeater {
                    model: socket.gpu
                    delegate: GpuCard { gpu: modelData }
                }

                Repeater {
                    model: fans
                    delegate: FanCard {
                        label: model.label
                        rpm: model.rpm
                        active: root.windowActive
                    }
                }

                RamCard { ram: socket.ram }
            }
        }
    }

    CloseButton {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 14
        anchors.rightMargin: 14
        onClicked: Qt.quit()
    }
}

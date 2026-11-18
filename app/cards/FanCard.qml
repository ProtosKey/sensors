import QtQuick
import QtQuick.Layouts
import "../components"

Card {
    id: card

    property string label
    property real rpm: 0
    property bool active: true // крутить значок только пока окно видно

    RowLayout {
        Layout.fillWidth: true
        spacing: 10

        FanIcon {
            rpm: card.rpm
            active: card.active
        }
        Text {
            text: label.replace(/_/g, " ").toUpperCase()
            color: Theme.muted
            font.pixelSize: 12
        }
        Item { Layout.fillWidth: true }
        SelectableText {
            text: Theme.num(rpm) + " RPM"
            font.pixelSize: 14
            font.bold: true
        }
    }
}

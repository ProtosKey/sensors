import QtQuick
import QtQuick.Layouts
import "../components"

Card {
    id: fanCard

    property real rpm: 0
    property string label
    property bool is_active: true

    RowLayout {
        Layout.fillWidth: true
        spacing: Theme.rowSpacing

        FanIcon {
            rpm: fanCard.rpm
            active: fanCard.is_active
        }
        Text {
            text: Theme.capitalize(fanCard.label.replace(/_/g, " "))
            color: Theme.muted
            font.pointSize: Theme.sizeLabel
        }
        Item {
            Layout.fillWidth: true
        }
        SelectableText {
            text: Theme.num(fanCard.rpm) + " RPM"
            font.pointSize: Theme.sizeValue
            font.bold: true
        }
    }
}

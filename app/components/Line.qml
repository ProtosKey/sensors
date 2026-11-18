import QtQuick
import QtQuick.Layouts

// Строка «подпись ........ значение» (с необязательной иконкой слева).
RowLayout {
    property string label
    property string value
    property color valueColor: Theme.text
    property string iconPath

    Layout.fillWidth: true
    spacing: 10

    Icon {
        visible: iconPath !== ""
        path: iconPath
    }
    Text {
        text: label
        color: Theme.muted
        font.pixelSize: 12
    }
    Item { Layout.fillWidth: true }
    SelectableText {
        text: value
        color: valueColor
        font.pixelSize: 14
        font.bold: true
    }
}

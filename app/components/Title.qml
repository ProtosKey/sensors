import QtQuick
import QtQuick.Layouts

// Заголовок карточки: иконка + название.
RowLayout {
    property string iconPath
    property string text

    Layout.fillWidth: true
    spacing: 10

    Icon { path: iconPath }
    Text {
        text: parent.text
        color: Theme.text
        font.pixelSize: 13
        font.bold: true
        elide: Text.ElideRight
        Layout.fillWidth: true
    }
}

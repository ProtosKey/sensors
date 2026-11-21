import QtQuick
import QtQuick.Layouts

RowLayout {
    id: title

    property string iconPath
    property string text

    Layout.fillWidth: true
    spacing: Theme.rowSpacing

    Icon {
        path: title.iconPath
    }
    SelectableText {
        text: title.text
        font.pointSize: Theme.sizeTitle
        font.bold: true
        Layout.fillWidth: true
    }
}

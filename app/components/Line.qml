import QtQuick
import QtQuick.Layouts

RowLayout {
    id: line

    property string label
    property string value
    property color valueColor: Theme.text
    property string iconPath

    Layout.fillWidth: true
    spacing: Theme.rowSpacing

    Icon {
        visible: line.iconPath !== ""
        path: line.iconPath
    }
    Text {
        text: line.label
        color: Theme.muted
        font.pointSize: Theme.sizeLabel
    }
    Item {
        Layout.fillWidth: true
    }
    SelectableText {
        text: line.value
        color: line.valueColor
        font.pointSize: Theme.sizeValue
        font.bold: true
    }
}

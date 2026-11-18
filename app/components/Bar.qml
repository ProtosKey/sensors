import QtQuick
import QtQuick.Layouts

// Тонкая полоса заполнения 0..1.
Rectangle {
    property real fraction: 0
    property bool warn: true // подсвечивать при высоком заполнении

    Layout.fillWidth: true
    Layout.topMargin: 2
    height: 5
    radius: 2.5
    color: Theme.mid

    Rectangle {
        width: parent.width * parent.fraction
        height: parent.height
        radius: parent.radius
        color: parent.warn ? Theme.loadColor(parent.fraction) : Theme.highlight
    }
}

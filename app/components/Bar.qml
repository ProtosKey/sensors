import QtQuick
import QtQuick.Layouts

Rectangle {
    id: bar

    property real fraction: 0
    property bool warning: true

    Layout.fillWidth: true
    Layout.topMargin: Theme.barMargin
    height: Theme.barHeight
    radius: height / 2
    color: Theme.border

    Rectangle {
        width: bar.fraction > 0 ? Math.max(bar.width * bar.fraction, bar.height) : 0
        height: bar.height
        radius: bar.radius
        color: bar.warning ? Theme.loadColor(bar.fraction) : Theme.highlight
    }
}

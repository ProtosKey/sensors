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
    color: Theme.border

    Rectangle {
        // Не уже собственной высоты: иначе радиус скругления не помещается, и
        // заливка торчит из закруглённого края дорожки квадратным углом.
        // Нулевое значение не рисуется вовсе.
        width: parent.fraction > 0 ? Math.max(parent.width * parent.fraction, parent.height) : 0
        height: parent.height
        radius: parent.radius
        color: parent.warn ? Theme.loadColor(parent.fraction) : Theme.highlight
    }
}

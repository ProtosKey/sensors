import QtQuick
import QtQuick.Layouts

// Карточка: всё, что положено внутрь, попадает во вложенную колонку.
Rectangle {
    default property alias content: column.data

    Layout.fillWidth: true
    implicitHeight: column.implicitHeight + 24
    radius: 10
    color: Theme.alternateBase
    border.color: Theme.border
    border.width: 1

    ColumnLayout {
        id: column
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 12
        spacing: 5
    }
}

import QtQuick
import QtQuick.Layouts

Rectangle {
    id: card

    default property alias content: column.data

    Layout.fillWidth: true
    implicitHeight: column.implicitHeight + 2 * Theme.cardPadding
    radius: Theme.cardRadius
    color: Theme.alternateBase
    border.color: Theme.border
    border.width: Theme.lineWidth

    ColumnLayout {
        id: column
        anchors.verticalCenter: card.verticalCenter
        anchors.left: card.left
        anchors.right: card.right
        anchors.margins: Theme.cardPadding
        spacing: Theme.cardSpacing
    }
}

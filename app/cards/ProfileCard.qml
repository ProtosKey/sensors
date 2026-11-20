import QtQuick
import "../components"

Card {
    id: profileCard

    property string profile

    Line {
        iconPath: Theme.iconProfile
        label: qsTr("Profile")
        value: profileCard.profile || "N/A"
    }
}

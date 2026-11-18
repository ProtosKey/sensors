import QtQuick
import "../components"

Card {
    property string profile

    Line {
        iconPath: Theme.iconProfile
        label: qsTr("Profile")
        value: profile || "N/A"
    }
}

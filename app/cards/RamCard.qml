import QtQuick
import "../components"

Card {
    // {used, total} в МиБ
    property var ram: ({})

    visible: ram.total !== undefined

    Line {
        iconPath: Theme.iconRam
        label: "RAM"
        value: Theme.gigabytes(ram.used) + " / " + Theme.gigabytes(ram.total) + " GB"
    }
    Bar {
        fraction: Theme.ratio(ram.used, ram.total)
    }
}

import QtQuick
import "../components"

Card {
    id: ramCard

    property var ram: ({})

    visible: ramCard.ram.total !== undefined

    Line {
        iconPath: Theme.iconRam
        label: "RAM"
        value: Theme.gibibytes(ramCard.ram.used) + " / " + Theme.gibibytes(ramCard.ram.total) + " GiB"
    }
    Bar {
        fraction: Theme.ratio(ramCard.ram.used, ramCard.ram.total)
    }
}

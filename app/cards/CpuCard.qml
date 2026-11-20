import QtQuick
import "../components"

Card {
    id: cpuCard

    property var cpu: ({})

    Title {
        iconPath: Theme.iconCpu
        text: (cpuCard.cpu.model || "N/A").replace(/\s+w\/.*$/, "")
    }
    Line {
        label: qsTr("Temperature")
        value: Theme.num(cpuCard.cpu.temp) + "°C"
        valueColor: Theme.tempColor(cpuCard.cpu.temp)
    }
    Line {
        visible: cpuCard.cpu.freq !== undefined
        label: qsTr("Frequency")
        value: Theme.num(cpuCard.cpu.freq) + " MHz"
    }
    Line {
        visible: cpuCard.cpu.power !== undefined
        label: qsTr("Power")
        value: Theme.num(cpuCard.cpu.power, 1) + " W"
    }
}

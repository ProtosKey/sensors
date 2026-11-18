import QtQuick
import "../components"

Card {
    // {model, temp, freq, power} из ответа демона
    property var cpu: ({})

    Title {
        iconPath: Theme.iconCpu
        text: (cpu.model || "N/A").replace(/\s+w\/.*$/, "")
    }
    Line {
        label: qsTr("Temperature")
        value: Theme.num(cpu.temp) + "°C"
        valueColor: Theme.tempColor(cpu.temp)
    }
    Line {
        visible: cpu.freq !== undefined
        label: qsTr("Frequency")
        value: Theme.num(cpu.freq) + " MHz"
    }
    Line {
        visible: cpu.power !== undefined
        label: qsTr("Power")
        value: Theme.num(cpu.power, 1) + " W"
    }
}

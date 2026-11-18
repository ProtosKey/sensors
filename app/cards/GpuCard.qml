import QtQuick
import "../components"

Card {
    // {name, temp, power, freq, active, vram: {used, total}, gtt: {...}}
    property var gpu: ({})

    Title {
        iconPath: Theme.iconGpu
        text: gpu.name || "GPU"
    }
    Line {
        visible: gpu.active === false
        label: qsTr("State")
        value: qsTr("Sleeping")
    }
    Line {
        visible: gpu.temp !== undefined
        label: qsTr("Temperature")
        value: Theme.num(gpu.temp) + "°C"
        valueColor: Theme.tempColor(gpu.temp)
    }
    Line {
        visible: gpu.power !== undefined
        label: qsTr("Power")
        value: Theme.num(gpu.power, 1) + " W"
    }
    Line {
        visible: gpu.freq !== undefined
        label: qsTr("Frequency")
        value: Theme.num(gpu.freq) + " MHz"
    }
    Line {
        visible: gpu.vram !== undefined
        label: "VRAM"
        value: gpu.vram ? Theme.num(gpu.vram.used) + " / " + Theme.num(gpu.vram.total) + " MB" : ""
    }
    Bar {
        visible: gpu.vram !== undefined
        warn: false // резерв BIOS почти всегда занят, это норма
        fraction: gpu.vram ? Theme.ratio(gpu.vram.used, gpu.vram.total) : 0
    }
    Line {
        visible: gpu.gtt !== undefined
        label: qsTr("GTT (system RAM)")
        value: gpu.gtt ? Theme.num(gpu.gtt.used) + " / " + Theme.num(gpu.gtt.total) + " MB" : ""
    }
    Bar {
        visible: gpu.gtt !== undefined
        fraction: gpu.gtt ? Theme.ratio(gpu.gtt.used, gpu.gtt.total) : 0
    }
}

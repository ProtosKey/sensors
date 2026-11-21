import QtQuick
import "../components"

Card {
    id: gpuCard

    property var gpu: ({})

    Title {
        iconPath: Theme.iconGpu
        text: gpuCard.gpu.name || "GPU"
    }
    Line {
        visible: gpuCard.gpu.active === false
        label: qsTr("State")
        value: qsTr("Sleeping")
    }
    Line {
        visible: gpuCard.gpu.temp !== undefined
        label: qsTr("Temperature")
        value: Theme.num(gpuCard.gpu.temp) + "°C"
        valueColor: Theme.tempColor(gpuCard.gpu.temp)
    }
    Line {
        visible: gpuCard.gpu.power !== undefined
        label: qsTr("Power")
        value: Theme.num(gpuCard.gpu.power, 1) + " W"
    }
    Line {
        visible: gpuCard.gpu.freq !== undefined
        label: qsTr("Frequency")
        value: Theme.num(gpuCard.gpu.freq) + " MHz"
    }
    Line {
        visible: gpuCard.gpu.vram !== undefined
        label: "VRAM"
        value: gpuCard.gpu.vram ? Theme.num(gpuCard.gpu.vram.used) + " / " + Theme.num(gpuCard.gpu.vram.total) + " MiB" : ""
    }
    Bar {
        visible: gpuCard.gpu.vram !== undefined
        warning: false
        fraction: gpuCard.gpu.vram ? Theme.ratio(gpuCard.gpu.vram.used, gpuCard.gpu.vram.total) : 0
    }
    Line {
        visible: gpuCard.gpu.gtt !== undefined
        label: qsTr("GTT")
        value: gpuCard.gpu.gtt ? Theme.num(gpuCard.gpu.gtt.used) + " / " + Theme.num(gpuCard.gpu.gtt.total) + " MiB" : ""
    }
    Bar {
        visible: gpuCard.gpu.gtt !== undefined
        fraction: gpuCard.gpu.gtt ? Theme.ratio(gpuCard.gpu.gtt.used, gpuCard.gpu.gtt.total) : 0
    }
}

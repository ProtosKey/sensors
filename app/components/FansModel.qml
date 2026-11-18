import QtQuick

// Вентиляторы, обновляемые "на месте". Если отдать Repeater новый список, он
// каждые 2 с пересоздаёт значки, и вращение сбрасывается. Здесь строки модели
// живут постоянно, меняются только числа.
ListModel {
    id: model

    // список вида [{label, rpm}, ...] (например, socket.fans)
    property var source: []

    onSourceChanged: sync()
    Component.onCompleted: sync()

    function sync() {
        const fans = source || []
        while (model.count > fans.length)
            model.remove(model.count - 1)
        for (let i = 0; i < fans.length; i++) {
            const fan = { label: String(fans[i].label), rpm: Number(fans[i].rpm) }
            if (i < model.count)
                model.set(i, fan)
            else
                model.append(fan)
        }
    }
}

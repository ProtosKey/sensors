import QtQuick

ListModel {
    id: fansModel

    property var source: []

    onSourceChanged: sync()
    Component.onCompleted: sync()

    function sync() {
        const fans = fansModel.source || []
        while (fansModel.count > fans.length)
            fansModel.remove(fansModel.count - 1)
        for (let i = 0; i < fans.length; i++) {
            const fan = { label: String(fans[i].label), rpm: Number(fans[i].rpm) }
            if (i < fansModel.count)
                fansModel.set(i, fan)
            else
                fansModel.append(fan)
        }
    }
}

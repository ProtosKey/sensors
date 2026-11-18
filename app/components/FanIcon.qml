import QtQuick

// Вентилятор: скорость вращения пропорциональна RPM (10 000 об/мин = максимум),
// 0 RPM -- стоит. Реальную скорость показать нельзя (кадры не успевают),
// поэтому оборотов в секунду на экране намного меньше, чем на самом деле.
Item {
    id: fan
    property real rpm: 0
    // вращать только когда окно видно, чтобы виджет сам не нагружал процессор
    property bool active: true

    readonly property real maxRpm: 10000
    readonly property real maxRevsPerSecond: 2

    // обороты в секунду; плавно догоняет новое значение (данные приходят раз в 2 с)
    property real speed: Math.min(Math.max(rpm, 0), maxRpm) / maxRpm * maxRevsPerSecond
    Behavior on speed { NumberAnimation { duration: 1500 } }

    // Рабочее колесо турбины: много тонких наклонённых лопастей вокруг втулки
    readonly property int bladeCount: 10
    readonly property real hubRadius: 4 // чётный диаметр (8 px): втулка встаёт ровно по центру 24x24
    readonly property real bladeLength: 8
    readonly property real bladeTilt: 35 // наклон лопасти относительно радиуса, градусы

    implicitWidth: 24
    implicitHeight: 24

    // Вращающаяся часть
    Item {
        id: blades
        anchors.fill: parent

        Repeater {
            model: fan.bladeCount
            delegate: Item {
                // нулевой по размеру "шарнир" в центре: лопасть поворачивается вокруг него
                x: blades.width / 2
                y: blades.height / 2
                rotation: index * 360 / fan.bladeCount

                Rectangle {
                    x: fan.hubRadius - 1 // начинается под втулкой, чтобы лопасть касалась её без зазора
                    y: -height / 2
                    width: fan.bladeLength
                    height: 1
                    radius: height / 2
                    color: Theme.text
                    opacity: 0.9
                    antialiasing: true
                    transformOrigin: Item.Left
                    rotation: fan.bladeTilt
                }
            }
        }
    }

    // Неподвижная сплошная втулка строго по центру (вне вращающейся части)
    Rectangle {
        anchors.centerIn: parent
        width: fan.hubRadius * 2
        height: width
        radius: width / 2
        color: Theme.text
        antialiasing: true
    }

    // Кадр за кадром (плавно) и только пока вентилятор крутится
    FrameAnimation {
        running: fan.active && fan.speed > 0.001
        onTriggered: blades.rotation = (blades.rotation + fan.speed * 360 * frameTime) % 360
    }
}

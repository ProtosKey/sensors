pragma Singleton
import QtQuick

// Общие цвета, форматирование чисел и контуры иконок.
QtObject {
    readonly property SystemPalette palette: SystemPalette {
        colorGroup: SystemPalette.Active
    }

    readonly property color text: palette.text
    readonly property color window: palette.window
    readonly property color mid: palette.mid
    readonly property color alternateBase: palette.alternateBase
    readonly property color highlight: palette.highlight
    readonly property color muted: Qt.alpha(palette.text, 0.65)

    readonly property color hot: "#e5534b"
    readonly property color warm: "#e0a030"
    readonly property color error: "#d9534f"

    // Контуры иконок. Холст 24x24 рисуется 1:1, линия 1 px лежит на координатах x.5
    // (чётко, без размытия), промежуток между соседними линиями везде 2 px (шаг 3).
    readonly property string iconProfile: "M3 7.5H5 M14 7.5H21 M7.5 5.5h4v4h-4z M3 12.5H12 M14.5 10.5h4v4h-4z M12 17.5H21 M5.5 15.5h4v4h-4z"
    readonly property string iconCpu: "M4.5 4.5h15v15h-15z M7.5 7.5h9v9h-9z M7.5 4v-2 M10.5 4v-2 M13.5 4v-2 M16.5 4v-2 M7.5 20v2 M10.5 20v2 M13.5 20v2 M16.5 20v2 M4 7.5h-2 M4 10.5h-2 M4 13.5h-2 M4 16.5h-2 M20 7.5h2 M20 10.5h2 M20 13.5h2 M20 16.5h2"
    readonly property string iconGpu: "M2.5 4.5h19v13h-19z M5.5 7.5h7v7h-7z M15.5 7.5h3v2h-3z M15.5 12.5h3v2h-3z M7.5 18v2 M10.5 18v2 M13.5 18v2 M16.5 18v2"
    readonly property string iconRam: "M1.5 6.5h21v9h-21z M4.5 9.5h3v3h-3z M10.5 9.5h3v3h-3z M16.5 9.5h3v3h-3z M4.5 16v2 M7.5 16v2 M16.5 16v2 M19.5 16v2"

    // --- Форматирование ---
    function num(value, digits) {
        return (value === undefined || value === null) ? "—" : Number(value).toFixed(digits ?? 0)
    }
    function gigabytes(mib) {
        return num(mib / 1024, 1)
    }
    function ratio(used, total) {
        return (total > 0) ? Math.min(1, used / total) : 0
    }
    function tempColor(celsius) {
        if (celsius >= 85) return hot
        if (celsius >= 70) return warm
        return text
    }
    function loadColor(fraction) {
        if (fraction >= 0.9) return hot
        if (fraction >= 0.75) return warm
        return highlight
    }
}

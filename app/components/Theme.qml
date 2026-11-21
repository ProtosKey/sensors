pragma Singleton
import QtQuick

QtObject {
    readonly property SystemPalette palette: SystemPalette {
        colorGroup: SystemPalette.Active
    }

    readonly property color text: palette.text
    readonly property color window: palette.window
    readonly property color border: Qt.tint(palette.window, Qt.alpha(palette.text, 0.15))
    readonly property color alternateBase: palette.alternateBase
    readonly property color highlight: palette.highlight
    readonly property color highlightedText: palette.highlightedText
    readonly property color muted: Qt.alpha(palette.text, 0.65)

    readonly property color hot: "#e5534b"
    readonly property color warm: "#e0a030"
    readonly property color error: "#d9534f"

    readonly property string iconProfile: "M1.5 5.5H3.5 M6.5 3.5h4v4h-4z M13.5 5.5H22.5 M1.5 12.5H10.5 M13.5 10.5h4v4h-4z M20.5 12.5H22.5 M1.5 19.5H6.5 M9.5 17.5h4v4h-4z M16.5 19.5H22.5"
    readonly property string iconCpu: "M4.5 4.5h15v15h-15z M7.5 7.5h9v9h-9z M7.5 4v-2 M10.5 4v-2 M13.5 4v-2 M16.5 4v-2 M7.5 20v2 M10.5 20v2 M13.5 20v2 M16.5 20v2 M4 7.5h-2 M4 10.5h-2 M4 13.5h-2 M4 16.5h-2 M20 7.5h2 M20 10.5h2 M20 13.5h2 M20 16.5h2"
    readonly property string iconGpu: "M1.5 3v17 M1.5 5.5h21v12h-21 M4.5 8.5h6v6h-6z M13.5 8.5h6v6h-6z M7.5 18v2 M10.5 18v2 M13.5 18v2 M16.5 18v2"
    readonly property string iconRam: "M1.5 6.5h21v9h-21z M4.5 9.5h3v3h-3z M10.5 9.5h3v3h-3z M16.5 9.5h3v3h-3z M4.5 16v2 M7.5 16v2 M16.5 16v2 M19.5 16v2"
    readonly property string iconClose: "M7.5 7.5L16.5 16.5 M16.5 7.5L7.5 16.5"

    readonly property real baseSize: Qt.application.font.pointSize > 0 ? Qt.application.font.pointSize : 10
    readonly property real fontScale: Math.max(1, baseSize / 10)
    readonly property real sizeHeader: baseSize * 1.35
    readonly property real sizeTitle: baseSize
    readonly property real sizeValue: baseSize * 1.05
    readonly property real sizeLabel: baseSize * 0.9
    readonly property real sizeNote: baseSize * 0.85

    readonly property int windowWidth: 340
    readonly property int windowRadius: 15
    readonly property int windowBorder: 1
    readonly property int windowPadding: 15
    readonly property int windowScreenMargin: 20
    readonly property int sectionSpacing: 8
    readonly property int noteMargin: 4

    readonly property int cardRadius: 10
    readonly property int cardPadding: 12
    readonly property int cardSpacing: 5
    readonly property int rowSpacing: 10

    readonly property int iconSize: 24
    readonly property int lineWidth: 1
    readonly property int barHeight: 5
    readonly property int barMargin: 2
    readonly property int closeButtonSize: 28
    readonly property int closeButtonMargin: 14

    readonly property real tempWarm: 70
    readonly property real tempHot: 85
    readonly property real loadWarm: 0.75
    readonly property real loadHot: 0.9

    function num(value, digits) {
        return (value === undefined || value === null) ? "—" : Number(value).toFixed(digits ?? 0)
    }
    function capitalize(text) {
        return text ? text.charAt(0).toUpperCase() + text.slice(1) : ""
    }
    function gibibytes(mib) {
        return num(mib / 1024, 1)
    }
    function ratio(used, total) {
        return (total > 0) ? Math.min(1, used / total) : 0
    }
    function tempColor(celsius) {
        if (celsius >= tempHot) return hot
        if (celsius >= tempWarm) return warm
        return text
    }
    function loadColor(fraction) {
        if (fraction >= loadHot) return hot
        if (fraction >= loadWarm) return warm
        return highlight
    }
}

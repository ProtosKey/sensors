import QtQuick
import QtQuick.Shapes

// Иконка-контур по SVG-пути на холсте 24x24.
Item {
    id: icon
    property string path

    implicitWidth: 24
    implicitHeight: 24

    Shape {
        width: 24
        height: 24
        preferredRendererType: Shape.CurveRenderer

        ShapePath {
            strokeColor: Theme.text
            strokeWidth: 1
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin
            PathSvg { path: icon.path }
        }
    }
}

import QtQuick
import QtQuick.Shapes

Item {
    id: icon
    property string path

    implicitWidth: Theme.iconSize
    implicitHeight: Theme.iconSize

    Shape {
        width: Theme.iconSize
        height: Theme.iconSize
        preferredRendererType: Shape.CurveRenderer

        ShapePath {
            strokeColor: Theme.text
            strokeWidth: Theme.lineWidth
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin
            PathSvg { path: icon.path }
        }
    }
}

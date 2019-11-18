import QtQuick 2.0
import CustomInspectorWidgets 1.0

CustomInspector {
    dataDict: {
        "Loader": ["name", "filename","componentState"],
        "Vectors": ["position", "edges", "triangles", "quads"],
        "Transformations": ["rotation", "scale3d", "translation", "transformation"],
    }
}

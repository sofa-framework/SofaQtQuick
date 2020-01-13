import QtQuick 2.0
import SofaApplication 1.0
import CustomInspectorWidgets 1.0

CustomInspector {
    property var component: SofaApplication.selectedComponent
    onComponentChanged: {
        if (!SofaApplication.selectedComponent)
            return;
        if (component !== SofaApplication.selectedComponent) {
            component = SofaApplication.selectedComponent;
            return;
        }

        var dataMap = {}
//        console.error(SofaApplication.selectedComponent)
        for (var idx in SofaApplication.selectedComponent.getDataFields()) {
            var dataName = SofaApplication.selectedComponent.getDataFields()[idx]
            var data = SofaApplication.selectedComponent.getData(dataName)
            if (data.properties.displayed || dataName === "gravity" || dataName === "bbox") {
                if (data.properties.readOnly && showAll == false)
                    continue
                if (data.group === "Infos")
                    continue
                var group = data.group ? data.group : data.ownerClass
                if (group === "" || group === "BaseObject")
                    group = "Base"
                if (dataMap[group] === undefined)
                    dataMap[group] = []
                dataMap[group].push(dataName)
            }
            else
            {
//                console.log(dataName)
            }
        }
        dataDict = dataMap

        for (idx in SofaApplication.selectedComponent.getLinks()) {
            var linkName = SofaApplication.selectedComponent.getLinks()[idx]
            linkList.push(linkName)
        }
    }

}




import QtQuick 2.0
import SofaApplication 1.0
import CustomInspectorWidgets 1.0

CustomInspector {
    component: SofaApplication.selectedComponent
    onComponentChanged: {
        var dataMap = {}
        for (var idx in component.getDataFields()) {
            var dataName = component.getDataFields()[idx]
            var data = component.getData(dataName)
            if (data.properties.displayed) {
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
        }
        dataDict = dataMap

        for (idx in component.getLinks()) {
            var linkName = component.getLinks()[idx]
            linkList.push(linkName)
        }
    }

}




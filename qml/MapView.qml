import QtQuick 2.15
import QtLocation 5.15
import QtPositioning 5.15

Item {
    anchors.fill: parent

    Plugin {
        id: mapPlugin
        name: "osm"  // OpenStreetMap, puedes cambiar a Mapbox si tienes API
    }

    Map {
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(4.60971, -74.08175)
        zoomLevel: 15

        // Puedes añadir puntos aquí
    }
}

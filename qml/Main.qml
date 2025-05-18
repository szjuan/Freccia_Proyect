import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 1280
    height: 800
    title: "Atenea UI - Visualización GPS"

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        padding: 10

        Button {
            text: "Cargar archivo CSV"
            onClicked: {
                // TODO: Llamar a backend
            }
        }

        TabView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Tab {
                title: "Tabla"
                Rectangle {
                    anchors.fill: parent
                    color: "#ffffff"
                    Text { text: "Aquí irá la tabla de datos" }
                }
            }

            Tab {
                title: "Gráfica"
                ChartView {
                    anchors.fill: parent
                }
            }

            Tab {
                title: "Mapa"
                MapView {
                    anchors.fill: parent
                }
            }
        }
    }
}

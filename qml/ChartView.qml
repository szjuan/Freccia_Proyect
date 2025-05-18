import QtQuick 2.15
import QtCharts 2.15

Item {
    anchors.fill: parent

    ChartView {
        anchors.fill: parent
        theme: ChartView.ChartThemeDark
        antialiasing: true

        LineSeries {
            name: "HDOP"
            // Aquí irán los puntos en tiempo real
        }
    }
}

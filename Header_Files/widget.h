#ifndef WIDGET_H
#define WIDGET_H

#include "SensorManager.h"

#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QVector3D>
#include <QtDataVisualization>

#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QScatter3DSeries>
#include <QtDataVisualization/QScatterDataProxy>
#include <QtDataVisualization/QScatterDataItem>

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    int xIndex = 0;

    // Gráficos por grupo
    QChart *chartAccel = nullptr;
    QChart *chartGyro = nullptr;
    QChart *chartGPS = nullptr;
    QChart *chartSat = nullptr;

    // Series por grupo
    QLineSeries *seriesAccX = nullptr;
    QLineSeries *seriesAccY = nullptr;
    QLineSeries *seriesAccZ = nullptr;
    QLineSeries *seriesGyroX = nullptr;
    QLineSeries *seriesGyroY = nullptr;
    QLineSeries *seriesGyroZ = nullptr;
    QLineSeries *seriesLat = nullptr;
    QLineSeries *seriesLon = nullptr;
    QLineSeries *seriesSats = nullptr;

    // Labels dinámicas
    QLabel *labelAccX = nullptr;
    QLabel *labelAccY = nullptr;
    QLabel *labelAccZ = nullptr;
    QLabel *labelGyroX = nullptr;
    QLabel *labelGyroY = nullptr;
    QLabel *labelGyroZ = nullptr;
    QLabel *labelLat = nullptr;
    QLabel *labelLon = nullptr;
    QLabel *labelSats = nullptr;

    QLabel* labelFecha = nullptr;
    QLabel* labelHora = nullptr;

    SensorManager* m_sensorManager = nullptr;

    Q3DScatter* scatterGraph = nullptr;
    QWidget* container3D = nullptr;
    QScatter3DSeries* scatterSeries = nullptr;
    QScatterDataProxy* scatterProxy = nullptr;

    // Ejes dinámicos para las gráficas
    QValueAxis *axisX1 = nullptr;
    QValueAxis *axisX2 = nullptr;
    QValueAxis *axisX3 = nullptr;
    QValueAxis *axisX4 = nullptr;

    QValueAxis *axisY1 = nullptr;
    QValueAxis *axisY2 = nullptr;
    QValueAxis *axisY3 = nullptr;
    QValueAxis *axisY4 = nullptr;
};

#endif

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
#include <QVector3D>
#include <QtDataVisualization>

// Qt Data Visualization (¡incluye directamente!)
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

    QLabel* labelFecha = nullptr;
    QLabel* labelHora = nullptr;

    SensorManager* m_sensorManager = nullptr;

    Q3DScatter* scatterGraph = nullptr;
    QWidget* container3D = nullptr;
    QScatter3DSeries* scatterSeries = nullptr;
    QScatterDataProxy* scatterProxy = nullptr;
};

#endif

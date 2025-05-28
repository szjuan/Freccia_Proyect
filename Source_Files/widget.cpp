#include "widget.h"
#include "SensorManager.h"
#include "SensorData.h"

#include <QDebug>
#include <QVector3D>
#include <QtDataVisualization/Q3DTheme>
#include <QTimer>
#include <QDateTime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("GUI_FRECCIA");
    setWindowIcon(QIcon(":/assets/logo_xae.png"));
    this->setStyleSheet("background-color: black;");

    auto layoutGlobal = new QHBoxLayout(this);
    auto mainLayout = new QGridLayout();

    auto aplicarEstiloGrafico = [](QChart* chart, QValueAxis* axisX, QValueAxis* axisY) {
        chart->setBackgroundBrush(QBrush(Qt::black));
        chart->setTitleBrush(QBrush(Qt::white));
        chart->legend()->setLabelColor(Qt::white);
        axisX->setLabelsColor(Qt::white);
        axisX->setTitleBrush(QBrush(Qt::white));
        axisY->setLabelsColor(Qt::white);
        axisY->setTitleBrush(QBrush(Qt::white));
    };

    // === ACCELEROMETERS ===
    chartAccel = new QChart();
    seriesAccX = new QLineSeries(); seriesAccX->setName("AccX");
    seriesAccY = new QLineSeries(); seriesAccY->setName("AccY");
    seriesAccZ = new QLineSeries(); seriesAccZ->setName("AccZ");
    chartAccel->addSeries(seriesAccX);
    chartAccel->addSeries(seriesAccY);
    chartAccel->addSeries(seriesAccZ);
    axisX1 = new QValueAxis(); axisX1->setTitleText("Tiempo");
    axisY1 = new QValueAxis(); axisY1->setTitleText("Aceleración");
    chartAccel->addAxis(axisX1, Qt::AlignBottom);
    chartAccel->addAxis(axisY1, Qt::AlignLeft);
    for (auto s : {seriesAccX, seriesAccY, seriesAccZ}) { s->attachAxis(axisX1); s->attachAxis(axisY1); }
    chartAccel->setTitle("Accelerometers");
    aplicarEstiloGrafico(chartAccel, axisX1, axisY1);
    QChartView *accelChartView = new QChartView(chartAccel);
    accelChartView->setMinimumSize(500, 300);
    mainLayout->addWidget(accelChartView, 0, 1);
    labelAccX = new QLabel("AccX: 0");
    labelAccY = new QLabel("AccY: 0");
    labelAccZ = new QLabel("AccZ: 0");
    for (auto lbl : {labelAccX, labelAccY, labelAccZ}) lbl->setStyleSheet("color: white; font-weight: bold;");
    auto layoutAcc = new QHBoxLayout(); layoutAcc->addWidget(labelAccX); layoutAcc->addWidget(labelAccY); layoutAcc->addWidget(labelAccZ);
    mainLayout->addLayout(layoutAcc, 1, 1);

    // === GYROS ===
    chartGyro = new QChart();
    seriesGyroX = new QLineSeries(); seriesGyroX->setName("GyroX");
    seriesGyroY = new QLineSeries(); seriesGyroY->setName("GyroY");
    seriesGyroZ = new QLineSeries(); seriesGyroZ->setName("GyroZ");
    chartGyro->addSeries(seriesGyroX);
    chartGyro->addSeries(seriesGyroY);
    chartGyro->addSeries(seriesGyroZ);
    axisX2 = new QValueAxis(); axisX2->setTitleText("Tiempo");
    axisY2 = new QValueAxis(); axisY2->setTitleText("Velocidad angular");
    chartGyro->addAxis(axisX2, Qt::AlignBottom);
    chartGyro->addAxis(axisY2, Qt::AlignLeft);
    for (auto s : {seriesGyroX, seriesGyroY, seriesGyroZ}) { s->attachAxis(axisX2); s->attachAxis(axisY2); }
    chartGyro->setTitle("Gyroscopes");
    aplicarEstiloGrafico(chartGyro, axisX2, axisY2);
    QChartView *gyroChartView = new QChartView(chartGyro);
    gyroChartView->setMinimumSize(500, 300);
    mainLayout->addWidget(gyroChartView, 0, 0);
    labelGyroX = new QLabel("GyroX: 0");
    labelGyroY = new QLabel("GyroY: 0");
    labelGyroZ = new QLabel("GyroZ: 0");
    for (auto lbl : {labelGyroX, labelGyroY, labelGyroZ}) lbl->setStyleSheet("color: white; font-weight: bold;");
    auto layoutGyro = new QHBoxLayout(); layoutGyro->addWidget(labelGyroX); layoutGyro->addWidget(labelGyroY); layoutGyro->addWidget(labelGyroZ);
    mainLayout->addLayout(layoutGyro, 1, 0);

    // === GPS ===
    chartGPS = new QChart();
    seriesLat = new QLineSeries(); seriesLat->setName("Latitud");
    seriesLon = new QLineSeries(); seriesLon->setName("Longitud");
    chartGPS->addSeries(seriesLat);
    chartGPS->addSeries(seriesLon);
    axisX3 = new QValueAxis(); axisX3->setTitleText("Tiempo");
    axisY3 = new QValueAxis(); axisY3->setTitleText("Coordenadas");
    chartGPS->addAxis(axisX3, Qt::AlignBottom);
    chartGPS->addAxis(axisY3, Qt::AlignLeft);
    for (auto s : {seriesLat, seriesLon}) { s->attachAxis(axisX3); s->attachAxis(axisY3); }
    chartGPS->setTitle("Altitude");
    aplicarEstiloGrafico(chartGPS, axisX3, axisY3);
    QChartView *gpsChartView = new QChartView(chartGPS);
    gpsChartView->setMinimumSize(500, 300);
    mainLayout->addWidget(gpsChartView, 2, 0);
    labelLat = new QLabel("Lat: 0");
    labelLon = new QLabel("Lon: 0");
    for (auto lbl : {labelLat, labelLon}) lbl->setStyleSheet("color: white; font-weight: bold;");
    auto layoutGps = new QHBoxLayout(); layoutGps->addWidget(labelLat); layoutGps->addWidget(labelLon);
    mainLayout->addLayout(layoutGps, 3, 0);

    // === SATELLITES ===
    chartSat = new QChart();
    seriesSats = new QLineSeries(); seriesSats->setName("Satélites conectados");
    chartSat->addSeries(seriesSats);
    axisX4 = new QValueAxis(); axisX4->setTitleText("Tiempo");
    axisY4 = new QValueAxis(); axisY4->setTitleText("Satélites");
    chartSat->addAxis(axisX4, Qt::AlignBottom);
    chartSat->addAxis(axisY4, Qt::AlignLeft);
    seriesSats->attachAxis(axisX4);
    seriesSats->attachAxis(axisY4);
    chartSat->setTitle("Pressure");
    aplicarEstiloGrafico(chartSat, axisX4, axisY4);
    QChartView *satChartView = new QChartView(chartSat);
    satChartView->setMinimumSize(500, 300);
    mainLayout->addWidget(satChartView, 2, 1);
    labelSats = new QLabel("Sats: 0");
    labelSats->setStyleSheet("color: white; font-weight: bold;");
    auto layoutSat = new QHBoxLayout(); layoutSat->addWidget(labelSats);
    mainLayout->addLayout(layoutSat, 3, 1);

    // Hora y Fecha
    labelHora = new QLabel("Hora: "); labelHora->setStyleSheet("color: white;");
    labelFecha = new QLabel("Fecha: "); labelFecha->setStyleSheet("color: white;");
    mainLayout->addWidget(labelHora, 4, 0);
    mainLayout->addWidget(labelFecha, 4, 1);

    layoutGlobal->addLayout(mainLayout, 3);

    // === GRAFICO 3D ===
    scatterGraph = new Q3DScatter();
    scatterGraph->activeTheme()->setType(Q3DTheme::ThemeRetro);
    scatterGraph->activeTheme()->setBackgroundColor(Qt::black);
    scatterGraph->activeTheme()->setLabelTextColor(Qt::white);
    scatterGraph->activeTheme()->setGridLineColor(Qt::gray);

    scatterSeries = new QScatter3DSeries();
    scatterProxy = new QScatterDataProxy();
    scatterSeries->setDataProxy(scatterProxy);
    scatterSeries->setItemSize(0.2f);
    scatterSeries->setBaseColor(Qt::green);
    scatterGraph->addSeries(scatterSeries);

    scatterGraph->axisX()->setTitle("Longitud");
    scatterGraph->axisY()->setTitle("Tiempo");
    scatterGraph->axisZ()->setTitle("Latitud");
    scatterGraph->axisX()->setTitleVisible(true);
    scatterGraph->axisY()->setTitleVisible(true);
    scatterGraph->axisZ()->setTitleVisible(true);

    container3D = QWidget::createWindowContainer(scatterGraph);
    container3D->setMinimumSize(QSize(400, 400));
    layoutGlobal->addWidget(container3D, 2);

    // SensorManager y conexión
    m_sensorManager = new SensorManager(this);
    connect(m_sensorManager, &SensorManager::newSensorData, this, [=](const SensorData& data) {
        seriesAccX->append(xIndex, data.accX);
        seriesAccY->append(xIndex, data.accY);
        seriesAccZ->append(xIndex, data.accZ);
        seriesGyroX->append(xIndex, data.gyroX);
        seriesGyroY->append(xIndex, data.gyroY);
        seriesGyroZ->append(xIndex, data.gyroZ);
        seriesLat->append(xIndex, data.latitude);
        seriesLon->append(xIndex, data.longitude);
        seriesSats->append(xIndex, data.satellites);

        labelAccX->setText("AccX: " + QString::number(data.accX, 'f', 2));
        labelAccY->setText("AccY: " + QString::number(data.accY, 'f', 2));
        labelAccZ->setText("AccZ: " + QString::number(data.accZ, 'f', 2));
        labelGyroX->setText("GyroX: " + QString::number(data.gyroX, 'f', 2));
        labelGyroY->setText("GyroY: " + QString::number(data.gyroY, 'f', 2));
        labelGyroZ->setText("GyroZ: " + QString::number(data.gyroZ, 'f', 2));
        labelLat->setText("Lat: " + QString::number(data.latitude, 'f', 6));
        labelLon->setText("Lon: " + QString::number(data.longitude, 'f', 6));
        labelSats->setText("Sats: " + QString::number(data.satellites));

        labelHora->setText("Hora: " + QString::number(data.utcHour));
        labelFecha->setText("Fecha: " + QString::fromStdString(data.date));

        QScatterDataArray *array = new QScatterDataArray();
        array->resize(1);
        (*array)[0].setPosition(QVector3D(data.longitude, xIndex, data.latitude));
        scatterProxy->addItems(*array);

        // Eje X dinámico (horizontal)
        if (xIndex > axisX1->max()) axisX1->setMax(xIndex);
        if (xIndex > axisX2->max()) axisX2->setMax(xIndex);
        if (xIndex > axisX3->max()) axisX3->setMax(xIndex);
        if (xIndex > axisX4->max()) axisX4->setMax(xIndex);

        // === Eje Y1 (Acelerómetro: AccX, AccY, AccZ) ===
        for (float acc : {data.accX, data.accY, data.accZ}) {
            if (acc > axisY1->max()) axisY1->setMax(acc);
            if (acc < axisY1->min()) axisY1->setMin(acc);
        }

        // === Eje Y2 (Giroscopio: GyroX, GyroY, GyroZ) ===
        for (float gyro : {data.gyroX, data.gyroY, data.gyroZ}) {
            if (gyro > axisY2->max()) axisY2->setMax(gyro);
            if (gyro < axisY2->min()) axisY2->setMin(gyro);
        }

        // === Eje Y3 (GPS: Latitud y Longitud) ===
        for (float coord : {data.latitude, data.longitude}) {
            if (coord > axisY3->max()) axisY3->setMax(coord);
            if (coord < axisY3->min()) axisY3->setMin(coord);
        }

        // === Eje Y4 (Satélites conectados) ===
        if (data.satellites > axisY4->max()) axisY4->setMax(data.satellites);
        if (data.satellites < axisY4->min()) axisY4->setMin(data.satellites);

        xIndex++;

    });
}

Widget::~Widget() {}

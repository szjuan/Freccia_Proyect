#include "widget.h"
#include "SensorManager.h"
#include "SensorData.h"

#include <QDebug>
#include <QVector3D>
#include <QtDataVisualization/Q3DTheme>

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

    // Acelerómetro
    chartAccel = new QChart();
    seriesAccX = new QLineSeries(); seriesAccX->setName("AccX");
    seriesAccY = new QLineSeries(); seriesAccY->setName("AccY");
    seriesAccZ = new QLineSeries(); seriesAccZ->setName("AccZ");
    chartAccel->addSeries(seriesAccX);
    chartAccel->addSeries(seriesAccY);
    chartAccel->addSeries(seriesAccZ);
    auto axisX1 = new QValueAxis(); axisX1->setRange(0, 1000); axisX1->setTitleText("Tiempo");
    auto axisY1 = new QValueAxis(); axisY1->setRange(-50, 50); axisY1->setTitleText("Aceleración");
    chartAccel->addAxis(axisX1, Qt::AlignBottom);
    chartAccel->addAxis(axisY1, Qt::AlignLeft);
    for (auto s : {seriesAccX, seriesAccY, seriesAccZ}) { s->attachAxis(axisX1); s->attachAxis(axisY1); }
    chartAccel->setTitle("Accelerometers");
    aplicarEstiloGrafico(chartAccel, axisX1, axisY1);
    mainLayout->addWidget(new QChartView(chartAccel), 0, 1);

    // Giroscopio
    chartGyro = new QChart();
    seriesGyroX = new QLineSeries(); seriesGyroX->setName("GyroX");
    seriesGyroY = new QLineSeries(); seriesGyroY->setName("GyroY");
    seriesGyroZ = new QLineSeries(); seriesGyroZ->setName("GyroZ");
    chartGyro->addSeries(seriesGyroX);
    chartGyro->addSeries(seriesGyroY);
    chartGyro->addSeries(seriesGyroZ);
    auto axisX2 = new QValueAxis(); axisX2->setRange(0, 1000); axisX2->setTitleText("Tiempo");
    auto axisY2 = new QValueAxis(); axisY2->setRange(-500, 500); axisY2->setTitleText("Velocidad angular");
    chartGyro->addAxis(axisX2, Qt::AlignBottom);
    chartGyro->addAxis(axisY2, Qt::AlignLeft);
    for (auto s : {seriesGyroX, seriesGyroY, seriesGyroZ}) { s->attachAxis(axisX2); s->attachAxis(axisY2); }
    chartGyro->setTitle("Gyroscopes");
    aplicarEstiloGrafico(chartGyro, axisX2, axisY2);
    mainLayout->addWidget(new QChartView(chartGyro), 0, 0);

    // GPS
    chartGPS = new QChart();
    seriesLat = new QLineSeries(); seriesLat->setName("Latitud");
    seriesLon = new QLineSeries(); seriesLon->setName("Longitud");
    chartGPS->addSeries(seriesLat);
    chartGPS->addSeries(seriesLon);
    auto axisX3 = new QValueAxis(); axisX3->setRange(0, 1000); axisX3->setTitleText("Tiempo");
    auto axisY3 = new QValueAxis(); axisY3->setRange(-200, 200); axisY3->setTitleText("Coordenadas");
    chartGPS->addAxis(axisX3, Qt::AlignBottom);
    chartGPS->addAxis(axisY3, Qt::AlignLeft);
    for (auto s : {seriesLat, seriesLon}) { s->attachAxis(axisX3); s->attachAxis(axisY3); }
    chartGPS->setTitle("Altitude");
    aplicarEstiloGrafico(chartGPS, axisX3, axisY3);
    mainLayout->addWidget(new QChartView(chartGPS), 1, 0);

    // Satélites
    chartSat = new QChart();
    seriesSats = new QLineSeries(); seriesSats->setName("Satélites conectados");
    chartSat->addSeries(seriesSats);
    auto axisX4 = new QValueAxis(); axisX4->setRange(0, 1000); axisX4->setTitleText("Tiempo");
    auto axisY4 = new QValueAxis(); axisY4->setRange(0, 20); axisY4->setTitleText("Satélites");
    chartSat->addAxis(axisX4, Qt::AlignBottom);
    chartSat->addAxis(axisY4, Qt::AlignLeft);
    seriesSats->attachAxis(axisX4);
    seriesSats->attachAxis(axisY4);
    chartSat->setTitle("Pressure");
    aplicarEstiloGrafico(chartSat, axisX4, axisY4);
    mainLayout->addWidget(new QChartView(chartSat), 1, 1);

    // Etiquetas
    labelHora = new QLabel("Hora: "); labelHora->setStyleSheet("color: white;");
    labelFecha = new QLabel("Fecha: "); labelFecha->setStyleSheet("color: white;");
    mainLayout->addWidget(labelHora, 2, 0);
    mainLayout->addWidget(labelFecha, 2, 1);

    layoutGlobal->addLayout(mainLayout, 3);

    // Gráfico 3D
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

    // SensorManager
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

        labelHora->setText("Hora: " + QString::number(data.utcHour));
        labelFecha->setText("Fecha: " + QString::fromStdString(data.date));

        QScatterDataArray *array = new QScatterDataArray();
        array->resize(1);
        (*array)[0].setPosition(QVector3D(data.longitude, xIndex, data.latitude));
        scatterProxy->addItems(*array);

        xIndex++;
    });
}

Widget::~Widget() {}

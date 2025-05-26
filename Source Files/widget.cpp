#include "widget.h"
#include "SensorManager.h"
#include "SensorData.h"

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QVBoxLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , m_chart(new QChart)
    , m_series(new QLineSeries)
{
    auto chartView = new QChartView(m_chart);
    m_chart->addSeries(m_series);

    auto axisX = new QValueAxis;
    axisX->setRange(0, 1000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Tiempo");

    auto axisY = new QValueAxis;
    axisY->setRange(-20, 20);  // Rango genérico (ajustable según datos)
    axisY->setTitleText("Valor");

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY);

    m_chart->legend()->hide();
    m_chart->setTitle("Sensor en tiempo real");

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);

    // Conectar el SensorManager
    m_sensorManager = new SensorManager(this);
    connect(m_sensorManager, &SensorManager::newSensorData, this, [=](const SensorData& data) {
        m_series->append(xIndex, data.accX);
        xIndex++;
    });
}

Widget::~Widget() {}

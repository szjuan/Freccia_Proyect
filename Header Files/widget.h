#ifndef WIDGET_H
#define WIDGET_H

#include "SensorManager.h"
#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QLabel>

QT_FORWARD_DECLARE_CLASS(QVBoxLayout)

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr); 
    ~Widget();

private:
    int xIndex = 0;

    QChart *m_chart = nullptr;
    QLineSeries *m_series = nullptr;

    SensorManager* m_sensorManager = nullptr;
};

#endif

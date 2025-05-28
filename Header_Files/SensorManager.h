#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <QObject>
#include "SensorData.h"
#include "SerialReader.h"
#include "DataCleaner.h"
#include <fstream>

class SensorManager : public QObject {
    Q_OBJECT

public:
    explicit SensorManager(QObject* parent = nullptr);
    void processRawData(const QByteArray& line);
    ~SensorManager();

signals:
    void newSensorData(const SensorData& data);

private:
    SerialReader* m_serialReader = nullptr;
    DataCleaner m_cleaner;
    std::ofstream rawFile;
};

#endif

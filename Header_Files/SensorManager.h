#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include "SensorData.h"
#include "SerialReader.h"
#include "DataCleaner.h"

#include <vector>
#include <fstream>
#include <QObject>

class SensorManager : public QObject {
    Q_OBJECT

public:
    explicit SensorManager(QObject* parent = nullptr);
    void processRawData(const QByteArray& line);
    ~SensorManager();

    const std::vector<SensorData>& getVectorData() const { return vectorData; }

signals:
    void newSensorData(const SensorData& data);

private:
    SerialReader* m_serialReader = nullptr;
    std::vector<SensorData> vectorData;
    DataCleaner cleaner;
};

#endif

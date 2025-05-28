#include "SensorManager.h"
#include <QStringList>
#include <fstream>
#include <filesystem>

SensorManager::SensorManager(QObject* parent) : QObject(parent) {
    m_serialReader = new SerialReader(this);
    connect(m_serialReader, &SerialReader::dataReceived, this, &SensorManager::processRawData);
    m_serialReader->start("COM6");

    rawFile.open("..\data\datos_raw.csv", std::ios::app);
    if (rawFile.tellp() == 0) {
        rawFile << "Latitud,Longitud,Fecha,HoraUTC,Segundos,Satélites,HDOP,Roll,Pitch,Yaw,Servo1,Servo2,Servo3,Servo4,AltDiff\n";
    }
}

void SensorManager::processRawData(const QByteArray& line) {
    QString str = QString::fromUtf8(line).trimmed();
    QStringList values = str.split(',');

    rawFile << str.toStdString() << "\n";

    if (values.size() < 15) return;

    try {
        // GPSData intermedio para limpiar y validar
        GPSData gps;
        gps.latitude = values[0].toDouble();
        gps.longitude = values[1].toDouble();
        gps.date = values[2].toStdString();
        gps.utc_time = values[3].toStdString();
        gps.satellites = values[5].toInt();
        gps.hdop = values[6].toFloat();

        std::vector<GPSData> gpsVector = { gps };
        m_cleaner.clean(gpsVector);

        for (const auto& d : gpsVector) {
            SensorData sensor;
            sensor.latitude = d.latitude;
            sensor.longitude = d.longitude;
            sensor.date = d.date;
            sensor.utcHour = QString::fromStdString(d.utc_time).toFloat();
            sensor.satellites = d.satellites;
            sensor.hdop = d.hdop;

            // Sensores físicos
            sensor.accX = values[8].toFloat();  // Pitch
            sensor.accY = values[7].toFloat();  // Roll
            sensor.accZ = 0;                    // No definido

            sensor.gyroX = 0;
            sensor.gyroY = 0;
            sensor.gyroZ = values[9].toFloat(); // Yaw

            emit newSensorData(sensor);
        }

    } catch (...) {
    }
}

SensorManager::~SensorManager() {
    if (rawFile.is_open()) rawFile.close();
}

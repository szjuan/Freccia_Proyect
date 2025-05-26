#include "SensorManager.h"
#include <QStringList>

SensorManager::SensorManager(QObject* parent) : QObject(parent) {
    m_serialReader = new SerialReader(this);
    connect(m_serialReader, &SerialReader::dataReceived, this, &SensorManager::processRawData);
    m_serialReader->start("COM6");
}

void SensorManager::processRawData(const QByteArray& line) {
    QString str = QString::fromUtf8(line).trimmed();
    QStringList values = str.split(',');

    if (values.size() < 11) return; // accX, accY, accZ, gyroX, gyroY, gyroZ, lat, lon, utc, sat, hdop

    try {
        // Paso 1: Convertir línea a GPSData (sólo campos GPS relevantes)
        GPSData gps;
        gps.latitude = values[6].toDouble();
        gps.longitude = values[7].toDouble();
        gps.utc_time = values[8].toStdString();  // mantener como string
        gps.satellites = values[9].toInt();
        gps.hdop = values[10].toFloat();
        gps.date = ""; // puedes llenar si lo tienes

        std::vector<GPSData> gpsVector = { gps };

        // Paso 2: Limpiar
        m_cleaner.clean(gpsVector);

        // Paso 3: Si quedaron datos limpios, conviértelos en SensorData y emítelos
        for (const auto& d : gpsVector) {
            SensorData sensor;
            sensor.accX = values[0].toFloat();
            sensor.accY = values[1].toFloat();
            sensor.accZ = values[2].toFloat();
            sensor.gyroX = values[3].toFloat();
            sensor.gyroY = values[4].toFloat();
            sensor.gyroZ = values[5].toFloat();
            sensor.latitude = d.latitude;
            sensor.longitude = d.longitude;
            sensor.utcHour = QString::fromStdString(d.utc_time).toFloat();  // convertir para graficar
            sensor.satellites = d.satellites;
            sensor.hdop = d.hdop;

            emit newSensorData(sensor);
        }

    } catch (...) {
        // Manejo de errores silencioso
    }
}

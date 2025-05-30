#include "SensorManager.h"
#include "DataCleaner.h"

#include <QSerialPortInfo>
#include <QStringList>
#include <QDebug>

SensorManager::SensorManager(QObject* parent) : QObject(parent) {
    m_serialReader = new SerialReader(this);
    connect(m_serialReader, &SerialReader::dataReceived, this, &SensorManager::processRawData);
    //m_serialReader->start("COM6");
}

void SensorManager::processRawData(const QByteArray& line) {
    QString str = QString::fromUtf8(line).trimmed();
    QStringList values = str.split(',');

    if (values.size() < 15) return;

    try {
        SensorData sensor;
        sensor.latitude    = values[0].toDouble();
        sensor.longitude   = values[1].toDouble();
        sensor.date        = values[2].toStdString();
        sensor.utc_time    = values[3].toStdString();
        sensor.secs        = values[4].toFloat();
        sensor.satellites  = values[5].toInt();
        sensor.hdop        = values[6].toFloat();
        sensor.Roll        = values[7].toFloat();
        sensor.Pitch       = values[8].toFloat();
        sensor.Yaw         = values[9].toFloat();
        sensor.Servo1      = values[10].toFloat();
        sensor.Servo2      = values[11].toFloat();
        sensor.Servo3      = values[12].toFloat();
        sensor.Servo4      = values[13].toFloat();
        sensor.AltDiff     = values[14].toFloat();
        sensor.pressure    = 0.0f;
        sensor.temperature = 0.0f;

        vectorData.push_back(sensor);

        // Limpieza y almacenamiento automático
        //cleaner.clean(vectorData);

        emit newSensorData(sensor);
    } catch (...) {
        // Ignorar errores de conversión
    }
}

SensorManager::~SensorManager() {}

QStringList SensorManager::listarPuertosDisponibles() {
    QStringList puertos;
    const auto puertosDisponibles = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& info : puertosDisponibles) {
        puertos << info.portName();
    }
    return puertos;
}

void SensorManager::iniciarLectura(const QString& puerto, int baudRate) {
    if (!m_serialReader) return;

    if (!puerto.isEmpty() && baudRate > 0) {
        m_serialReader->start(puerto, baudRate);
    } else {
        qWarning() << "Puerto o baudrate inválido";
    }
}
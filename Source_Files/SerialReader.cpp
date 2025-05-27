#include "SerialReader.h"
#include <QDebug>

SerialReader::SerialReader(QObject* parent) : QObject(parent) {
    m_serialPort = new QSerialPort(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialReader::handleReadyRead);
}

void SerialReader::start(const QString& portName) {
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(QSerialPort::Baud9600);  // ajusta si usas otra velocidad
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serialPort->open(QIODevice::ReadOnly)) {
        qCritical() << "No se pudo abrir el puerto:" << portName << "-" << m_serialPort->errorString();
    } else {
        qDebug() << "Puerto abierto correctamente:" << portName;
    }
}

void SerialReader::handleReadyRead() {
    const QByteArray data = m_serialPort->readLine();
    if (!data.isEmpty()) {
        emit dataReceived(data);
    }
}

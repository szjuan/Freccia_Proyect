#include "SerialReader.h"
#include <QDebug>

SerialReader::SerialReader(QObject* parent) : QObject(parent) {
    m_serialPort = new QSerialPort(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialReader::handleReadyRead);
}

void SerialReader::start(const QString& portName, int baudRate) {
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
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
    if (m_serialPort && m_serialPort->canReadLine()) {
        const QByteArray data = m_serialPort->readLine();
        if (!data.isEmpty()) {
            emit dataReceived(data);
        }
    }
}

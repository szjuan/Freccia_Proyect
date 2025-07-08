#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialReader : public QObject {
    Q_OBJECT

public:
    explicit SerialReader(QObject* parent = nullptr);
    void start(const QString& portName);

signals:
    void dataReceived(const QByteArray& data);

private slots:
    void handleReadyRead();

private:
    QSerialPort* m_serialPort = nullptr;
};

#endif

#include "DataRepository.h"
#include <fstream>
#include <sstream>
#include <QDebug>

DataRepository* DataRepository::instance() {
    static DataRepository* instancia = new DataRepository();
    return instancia;
}

DataRepository::DataRepository(QObject* parent) : QObject(parent) {}

void DataRepository::agregarDatoEnVivo(const SensorData& d) {
    if (modoActual == Modo::HISTORICO) return;

    modoActual = Modo::EN_VIVO;
    datos.push_back(d);

    qDebug() << "Nuevo dato en vivo agregado:" << d.latitude << d.longitude;
    emit newData(d);
}

bool DataRepository::cargarDesdeArchivo(const QString& ruta) {
    if (modoActual == Modo::EN_VIVO) return false;
    modoActual = Modo::HISTORICO;

    datos.clear();
    std::ifstream file(ruta.toStdString());
    if (!file.is_open()) return false;

    std::string linea;
    std::getline(file, linea);

    while (std::getline(file, linea)) {
        std::stringstream ss(linea);
        std::string valor;
        SensorData d;

        std::getline(ss, valor, ','); d.latitude = std::stod(valor);
        std::getline(ss, valor, ','); d.longitude = std::stod(valor);
        std::getline(ss, valor, ','); d.date = valor;
        std::getline(ss, valor, ','); d.utc_time = valor;
        std::getline(ss, valor, ','); d.secs = std::stof(valor);
        std::getline(ss, valor, ','); d.satellites = std::stoi(valor);
        std::getline(ss, valor, ','); d.hdop = std::stof(valor);
        std::getline(ss, valor, ','); d.Roll = std::stof(valor);
        std::getline(ss, valor, ','); d.Pitch = std::stof(valor);
        std::getline(ss, valor, ','); d.Yaw = std::stof(valor);
        std::getline(ss, valor, ','); d.Servo1 = std::stof(valor);
        std::getline(ss, valor, ','); d.Servo2 = std::stof(valor);
        std::getline(ss, valor, ','); d.Servo3 = std::stof(valor);
        std::getline(ss, valor, ','); d.Servo4 = std::stof(valor);
        std::getline(ss, valor, ','); d.AltDiff = std::stof(valor);
        std::getline(ss, valor, ','); d.pressure = std::stof(valor);
        std::getline(ss, valor, ','); d.temperature = std::stof(valor);

        datos.push_back(d);
        emit newData(d);
    }

    return true;
}

const std::vector<SensorData>& DataRepository::obtenerDatos() const {
    return datos;
}

DataRepository::Modo DataRepository::obtenerModo() const {
    return modoActual;
}

void DataRepository::reiniciar() {
    datos.clear();
    modoActual = Modo::NINGUNO;
}

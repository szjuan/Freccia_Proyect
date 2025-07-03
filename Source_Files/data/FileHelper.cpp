#include "FileHelper.h"

#include <fstream>
#include <filesystem>
#include <QDateTime>

bool FileHelper::exists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

#include <filesystem>

void FileHelper::ensureExists(const std::string& path) {
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());  // crea el folder si no existe
    if (!exists(path)) {
        std::ofstream file(path);
        file.close();
    }
}

void FileHelper::writeHeaderIfNew(const std::string& path, const std::string& header) {
    std::ifstream file(path);
    if (file.peek() == std::ifstream::traits_type::eof()) {
        std::ofstream outFile(path);
        outFile << header << "\n";
        outFile.close();
    }
}

void FileHelper::createDataDirectoryIfNeeded() {
    std::filesystem::create_directories("../data");
}

void FileHelper::appendRawData(const std::string& path, const SensorData& d) {
    std::ofstream file(path, std::ios::app);
    file << d.latitude << "," << d.longitude << "," << d.date << "," << d.utc_time << "," << d.secs << ","
         << d.satellites << "," << d.hdop << "," << d.Roll << "," << d.Pitch << "," << d.Yaw << ","
         << d.Servo1 << "," << d.Servo2 << "," << d.Servo3 << "," << d.Servo4 << "," << d.AltDiff << ","
         << d.pressure << "," << d.temperature << "\n";
    file.close();
}

void FileHelper::appendCleanData(const std::string& path, const std::vector<SensorData>& data) {
    std::ofstream file(path, std::ios::app);
    for (const auto& d : data) {
        file << d.latitude << "," << d.longitude << "," << d.date << "," << d.utc_time << "," << d.secs << ","
             << d.satellites << "," << d.hdop << "," << d.Roll << "," << d.Pitch << "," << d.Yaw << ","
             << d.Servo1 << "," << d.Servo2 << "," << d.Servo3 << "," << d.Servo4 << "," << d.AltDiff << ","
             << d.pressure << "," << d.temperature << "\n";
    }
    file.close();
}

void FileHelper::appendErrorData(const std::string& path, const SensorData& d, const std::string& errorDetail) {
    std::ofstream file(path, std::ios::app);
    file << d.latitude << "," << d.longitude << "," << d.date << "," << d.utc_time << ","
         << d.satellites << "," << d.hdop << "," << errorDetail << "\n";
    file.close();
}

void FileHelper::iniciarGrabacion() {
    createDataDirectoryIfNeeded();

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    rutaArchivoActual = "../data/rec_" + timestamp.toStdString() + ".csv";

    archivoGrabacion.open(rutaArchivoActual, std::ios::out);
    archivoGrabacion << "Latitud,Longitud,Fecha,Hora,Segundos,Satélites,HDOP,Roll,Pitch,Yaw,Servo1,Servo2,Servo3,Servo4,AltDiff,Presión,Temperatura\n";
}

void FileHelper::escribirDuranteGrabacion(const SensorData& d) {
    if (archivoGrabacion.is_open()) {
        archivoGrabacion << d.latitude << "," << d.longitude << "," << d.date << "," << d.utc_time << "," << d.secs << ","
                         << d.satellites << "," << d.hdop << "," << d.Roll << "," << d.Pitch << "," << d.Yaw << ","
                         << d.Servo1 << "," << d.Servo2 << "," << d.Servo3 << "," << d.Servo4 << "," << d.AltDiff << ","
                         << d.pressure << "," << d.temperature << "\n";
    }
}

void FileHelper::detenerGrabacion() {
    if (archivoGrabacion.is_open()) {
        archivoGrabacion.close();
    }
}
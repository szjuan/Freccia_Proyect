#ifndef FILEHELPER_H
#define FILEHELPER_H

#include "SensorData.h"

#include <string>
#include <vector>
#include <fstream>
#include <QTime>

class FileHelper {
public:
    static bool exists(const std::string& path);
    static void ensureExists(const std::string& path);
    static void writeHeaderIfNew(const std::string& path, const std::string& header);
    static void appendRawData(const std::string& path, const SensorData& data);
    static void appendCleanData(const std::string& path, const std::vector<SensorData>& data);
    static void appendErrorData(const std::string& path, const SensorData& data, const std::string& errorDetail);
    static void createDataDirectoryIfNeeded();

    void iniciarGrabacion();
    void escribirDuranteGrabacion(const SensorData& d);
    void detenerGrabacion();
    QTime tiempoGrabado;

private:
    std::ofstream archivoGrabacion;
    std::string rutaArchivoActual;
    
};

#endif

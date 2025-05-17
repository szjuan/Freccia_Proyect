#include "DataCleaner.h"
#include "FileHelper.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>

// Funciones auxiliares
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

float computeAccMagnitude(const SensorData& d) {
    return std::sqrt(d.acc_x * d.acc_x + d.acc_y * d.acc_y + d.acc_z * d.acc_z);
}

void DataCleaner::clean(std::vector<SensorData>& data) {
    std::vector<SensorData> cleaned;
    const std::string errorFilePath = "../data/errores.csv";
    const std::string cleanFilePath = "../data/clean_data.csv";
    int errores = 0;
    std::string timestamp = getCurrentTimestamp();

    FileHelper::ensureExists(errorFilePath);
    FileHelper::ensureExists(cleanFilePath);

    std::ofstream errorFile(errorFilePath, std::ios::app);
    if (!errorFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo errores.csv\n";
        return;
    }

    errorFile << "\n------------------Procesamiento: " << timestamp << " ------------------ \n";

    float prevTimestamp = -1.0;
    std::vector<float> acc_magnitudes;
    for (const auto& d : data) acc_magnitudes.push_back(computeAccMagnitude(d));

    float sum = std::accumulate(acc_magnitudes.begin(), acc_magnitudes.end(), 0.0f);
    float mean = sum / acc_magnitudes.size();
    float sq_sum = std::inner_product(acc_magnitudes.begin(), acc_magnitudes.end(), acc_magnitudes.begin(), 0.0f);
    float stdev = std::sqrt(sq_sum / acc_magnitudes.size() - mean * mean);

    for (size_t i = 0; i < data.size(); ++i) {
        const auto& d = data[i];
        bool valid = true;

        if (std::isnan(d.acc_x) || std::isnan(d.acc_y) || std::isnan(d.acc_z) ||
            std::isnan(d.gyro_x) || std::isnan(d.gyro_y) || std::isnan(d.gyro_z))
            valid = false;

        if (std::abs(d.acc_x) > 100 || std::abs(d.acc_y) > 100 || std::abs(d.acc_z) > 100 ||
            std::abs(d.gyro_x) > 100 || std::abs(d.gyro_y) > 100 || std::abs(d.gyro_z) > 100)
            valid = false;

        if (prevTimestamp >= 0 && d.timestamp < prevTimestamp)
            valid = false;

        float acc_mag = acc_magnitudes[i];
        float zscore = (acc_mag - mean) / stdev;
        if (std::abs(zscore) > 3)
            valid = false;

        if (valid) {
            cleaned.push_back(d);
            prevTimestamp = d.timestamp;
            std::cout << "t=" << d.timestamp
                      << " Acc=(" << d.acc_x << ", " << d.acc_y << ", " << d.acc_z << ")"
                      << " Gyro=(" << d.gyro_x << ", " << d.gyro_y << ", " << d.gyro_z << ")\n";
        } else {
            ++errores;
            errorFile << d.timestamp << ","
                      << d.acc_x << "," << d.acc_y << "," << d.acc_z << ","
                      << d.gyro_x << "," << d.gyro_y << "," << d.gyro_z << "\n";
        }
    }

    errorFile << "Número de datos con errores: " << errores << "\n";
    errorFile.close();

    std::cout << "Número de datos con errores: " << errores << "\n";

    std::ofstream cleanFile(cleanFilePath, std::ios::trunc);
    if (!cleanFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo clean_data.csv\n";
        return;
    }

    cleanFile << "\n------------------Procesamiento Datos: " << timestamp << "------------------ \n";
    cleanFile << "timestamp,acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z\n";

    for (const auto& d : cleaned) {
        cleanFile << d.timestamp << ","
                  << d.acc_x << "," << d.acc_y << "," << d.acc_z << ","
                  << d.gyro_x << "," << d.gyro_y << "," << d.gyro_z << "\n";
    }

    cleanFile.close();
    data = cleaned;
    data.clear();
}

void DataCleaner::applyMovingAverage(std::vector<SensorData>& data, int windowSize) {
    if (data.size() < windowSize) return;
    std::vector<SensorData> smoothed = data;

    for (size_t i = windowSize - 1; i < data.size(); ++i) {
        float sumAccX = 0, sumAccY = 0, sumAccZ = 0;
        float sumGyroX = 0, sumGyroY = 0, sumGyroZ = 0;

        for (int j = 0; j < windowSize; ++j) {
            sumAccX  += data[i - j].acc_x;
            sumAccY  += data[i - j].acc_y;
            sumAccZ  += data[i - j].acc_z;
            sumGyroX += data[i - j].gyro_x;
            sumGyroY += data[i - j].gyro_y;
            sumGyroZ += data[i - j].gyro_z;
        }

        smoothed[i].acc_x  = sumAccX / windowSize;
        smoothed[i].acc_y  = sumAccY / windowSize;
        smoothed[i].acc_z  = sumAccZ / windowSize;
        smoothed[i].gyro_x = sumGyroX / windowSize;
        smoothed[i].gyro_y = sumGyroY / windowSize;
        smoothed[i].gyro_z = sumGyroZ / windowSize;
    }

    data = smoothed;
}

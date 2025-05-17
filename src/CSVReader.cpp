#include "CSVReader.h"
#include <fstream>
#include <sstream>

CSVReader::CSVReader(const std::string& filename)
    : m_filename(filename) {}

std::vector<SensorData> CSVReader::readAll() {
    std::vector<SensorData> data;
    std::ifstream file(m_filename);

    if (!file.is_open()) {
        return data;
    }

    std::string line;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false; // Saltar encabezado
            continue;
        }

        std::stringstream ss(line);
        std::string token;
        std::vector<float> values;

        bool parseSuccess = true;
        while (std::getline(ss, token, ',')) {
            try {
                values.push_back(std::stof(token));
            } catch (...) {
                parseSuccess = false;
                break;
            }
        }

        // Línea inválida por parseo o longitud incorrecta
        if (!parseSuccess || values.size() != 7) {
            continue;
        }

        SensorData sd;
        sd.timestamp = values[0];
        sd.acc_x     = values[1];
        sd.acc_y     = values[2];
        sd.acc_z     = values[3];
        sd.gyro_x    = values[4];
        sd.gyro_y    = values[5];
        sd.gyro_z    = values[6];

        data.push_back(sd);
    }

    file.close();
    return data;
}

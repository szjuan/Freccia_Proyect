#pragma once

#include <vector>
#include <string>
#include "SensorData.h"

class CSVReader {
public:
    CSVReader(const std::string& filename);
    std::vector<SensorData> readAll();

private:
    std::string m_filename;
};

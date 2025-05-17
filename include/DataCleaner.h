#pragma once
#include <vector>
#include "SensorData.h"

class DataCleaner {
public:
    static void clean(std::vector<SensorData>& data);
    static void applyMovingAverage(std::vector<SensorData>& data, int windowSize = 5);
};

#ifndef SENSORDATA_H
#define SENSORDATA_H

struct SensorData {
    float accX;
    float accY;
    float accZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    double latitude;
    double longitude;
    float utcHour;
    int satellites;
    float hdop;
    std::string date;
};

#endif

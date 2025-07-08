#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <string>

struct SensorData {
    double latitude;
    double longitude;
    std::string date;
    std::string utc_time;
    float secs;
    int satellites;
    float hdop;
    float Roll;
    float Pitch;
    float Yaw;
    float Servo1;
    float Servo2;
    float Servo3;
    float Servo4;
    float AltDiff;
    float pressure;
    float temperature;
};

#endif

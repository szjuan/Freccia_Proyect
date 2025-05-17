#include <iostream>
#include "CSVReader.h"
#include "DataCleaner.h"

int main() {
   CSVReader reader("../data/sample_data.csv");
    auto datos = reader.readAll();

    DataCleaner::clean(datos);
    
    for (const auto& d : datos) {
        std::cout << "t=" << d.timestamp
                  << " Acc=(" << d.acc_x << ", " << d.acc_y << ", " << d.acc_z << ")"
                  << " Gyro=(" << d.gyro_x << ", " << d.gyro_y << ", " << d.gyro_z << ")"
                  << std::endl;
    }

    return 0;
}
